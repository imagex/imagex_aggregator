<?php
/**
 * @file
 * The Feed Item Processor for ImageX Aggregator
 */

/**
 * The Processor Function
 * 
 * @param $feed
 * The feed object
 */
function _imagex_aggregator_aggregator_process($feed) {
  imagex_aggregator_feed_load($feed);
  $items = array();
  foreach ($feed->items as $item) {
    $passed = FALSE;
    foreach ($feed->imagex['filters'] as $filter) {
      if (!_imagex_aggregator_filter($filter, $item)) {
        break;
      }
      // If we made it through the filtering without a break then we've passed
      $passed = TRUE;
    }
    if (empty($feed->imagex['filters'])) {
      // Automatically pass the filters if the feed didn't have any set
      $passed = TRUE;
    }
    if (!$passed) {
      // The filters didn't pass continue on to the next item
      continue;
    }
    $items[] = $item;
    // This first part of this code is taken directly from aggregator_process
    // Save this item. Try to avoid duplicate entries as much as possible. If
    // we find a duplicate entry, we resolve it and pass along its ID is such
    // that we can update it if needed.
    if (!empty($item['guid'])) {
      $entry = db_query("SELECT iid, timestamp FROM {aggregator_item} WHERE fid = :fid AND guid = :guid", array(':fid' => $feed->fid, ':guid' => $item['guid']))->fetchObject();
    }
    elseif ($item['link'] && $item['link'] != $feed->link && $item['link'] != $feed->url) {
      $entry = db_query("SELECT iid, timestamp FROM {aggregator_item} WHERE fid = :fid AND link = :link", array(':fid' => $feed->fid, ':link' => $item['link']))->fetchObject();
    }
    else {
      $entry = db_query("SELECT iid, timestamp FROM {aggregator_item} WHERE fid = :fid AND title = :title", array(':fid' => $feed->fid, ':title' => $item['title']))->fetchObject();
    }
    if (!$item['timestamp']) {
      $item['timestamp'] = isset($entry->timestamp) ? $entry->timestamp : REQUEST_TIME;
    }

    // Make sure the item title and author fit in the 255 varchar column.
    $item['title'] = truncate_utf8($item['title'], 255, TRUE, TRUE);
    $item['author'] = truncate_utf8($item['author'], 255, TRUE, TRUE);
    // End of aggregator code and start of imagex
    $edit = array('iid' => (isset($entry->iid) ? $entry->iid : ''), 'fid' => $feed->fid, 'timestamp' => $item['timestamp'], 'title' => $item['title'], 'link' => $item['link'], 'author' => $item['author'], 'description' => $item['description'], 'guid' => $item['guid']);
    _aggregator_save_item($edit);
    
    $item['iid'] = $edit['iid'];
    $update = db_query("SELECT iid FROM {imagex_aggregator_item} WHERE fid = :fid AND iid = :iid", array(':fid' => $feed->fid, ':iid' => $item['iid']))->fetchField();
    
    // We need to check the iid is valid because for some reson the feed inserts the rss channel node as an item
    if (!$update && is_numeric($item['iid'])) {
      // This is not an existing item so we need to create a entity for it.
      // First we need to get the bundle key so we can create a proper entity
      $entity_info = entity_get_info($feed->imagex['entity_type']);
      $bundle_key = isset($entity_info['bundle_keys']['bundle']) ? $entity_info['bundle_keys']['bundle'] : 'type';
            
      // We now make sure our field is added to this entity bundle
      if (!empty($entity_info['fieldable'])) {
        imagex_aggregator_add_field_instance($feed->imagex['entity_type'], $feed->imagex['bundle']);
      }
      
      // Now we create our entity and then an entity wrapper for ease of seting values
      $entity = entity_create($feed->imagex['entity_type'], array($bundle_key => $feed->imagex['bundle']));
      $wrapper = entity_metadata_wrapper($feed->imagex['entity_type'], $entity);
      
      // Then we walk through each mapping and set the data
      foreach ($feed->imagex['mapping'] as $map) {
        // We need to go through the fields and properties that were mapped and save them into the entity wrapper
        if (strpos($map['field'], 'field::') === 0) {
          // This is a field
          // We list out the exploded array skipping the first item in the array (field::) to get the field name and column key
          list(,$field, $column) = explode('::', $map['field']);
          $key = $map['item_key'];
          $value = $item[$key];
          drupal_alter('imagex_aggregator_field_presave', $value, $field, $feed->imagex['entity_type'], $feed->imagex['bundle']);
          try {
            // We need to wrap field values in arrays
            $wrapper->{$field}->set(array($column => $value));
          }
          catch (Exception $e) {
            drupal_set_message(t('You have tried to inset data into an entity that it wasn\'t expecting. For example putting a letter value where an integer was needed. Please verify the mapping you have set'), 'error');
            // Remove the item form the aggregator storage since it was not processed
            db_delete('aggregator_item')
              ->condition('iid', $item['iid'])
              ->execute();
            db_delete('aggregator_category_item')
              ->condition('iid', $item['iid'])
              ->execute();
            continue;
          }
          
        }
        else {
          // This is a property
          $field = $map['field'];
          $key = $map['item_key'];
          $value = $item[$key];
          drupal_alter('imagex_aggregator_property_presave', $value, $field, $feed->imagex['entity_type'], $feed->imagex['bundle']);
          $wrapper->{$field} = $value;
        }
      }
      try {
        // We need to set the imagex_aggregator field
        $wrapper->field_imagex_aggregator->set(1);
        
        // Invoke a hook before saving
        module_invoke_all('imagex_aggregator_entity_presave', $wrapper, $item, $feed);
        
        // Lastly we save the entity
        $wrapper->save();
        
        // Invoke a hook after saving
        module_invoke_all('imagex_aggregator_entity_save', $wrapper, $item, $feed);
      }
      catch (Exception $e) {
        drupal_set_message(t('There was an error saving an entity. This most likely due to your mappings. Please verify the mapping you have set'), 'error');
        // Remove the item form the aggregator storage since it was not processed
        db_delete('aggregator_item')
          ->condition('iid', $item['iid'])
          ->execute();
        db_delete('aggregator_category_item')
          ->condition('iid', $item['iid'])
          ->execute();
        continue;
      }
      
      $id = $wrapper->getIdentifier();
      if (!empty($id)) {
        // Only save the item if the entity was saved
        db_insert('imagex_aggregator_item')
          ->fields(array(
            'fid' => $feed->fid,
            'iid' => $item['iid'],
            'entity_type' => $feed->imagex['entity_type'],
            'bundle' => $feed->imagex['bundle'],
            'entity_id' => $id,
            'link' => $item['link'],
            'data' => serialize($item),
          ))
          ->execute();
      }
    }
  }
}

/**
 * Adds/edits/deletes an aggregator item.
 * This code is taken directly from aggregator module except we have
 * passed the $edit variable by reference.
 *
 * @param $edit
 *   An associative array describing the item to be added/edited/deleted.
 */
function _aggregator_save_item(&$edit) {
  if ($edit['title'] && empty($edit['iid'])) {
    $edit['iid'] = db_insert('aggregator_item')
      ->fields(array(
        'title' => $edit['title'],
        'link' => $edit['link'],
        'author' => $edit['author'],
        'description' => $edit['description'],
        'guid' => $edit['guid'],
        'timestamp' => $edit['timestamp'],
        'fid' => $edit['fid'],
      ))
      ->execute();
  }
  if ($edit['iid'] && !$edit['title']) {
    db_delete('aggregator_item')
      ->condition('iid', $edit['iid'])
      ->execute();
    db_delete('aggregator_category_item')
      ->condition('iid', $edit['iid'])
      ->execute();
  }
  elseif ($edit['title'] && $edit['link']) {
    // file the items in the categories indicated by the feed
    $result = db_query('SELECT cid FROM {aggregator_category_feed} WHERE fid = :fid', array(':fid' => $edit['fid']));
    foreach ($result as $category) {
      db_merge('aggregator_category_item')
        ->key(array(
          'iid' => $edit['iid'],
          'cid' => $category->cid,
        ))
        ->execute();
    }
  }
}

/**
 * Filters an item based on the filters provided
 * 
 * @param $filter
 * The array of filter data (item_key, condition, value)
 * 
 * @param $item
 * The Item to filter
 * 
 * @return
 * Either TRUE is all filters have passed or FALSE if one of them doesn't
 */
function _imagex_aggregator_filter($filter, $item) {
  $key = $filter['item_key'];
  switch ($filter['condition']) {
    case 'contains':
      // Search for the string in the item key. We make this case insensitive just to be safe
      return preg_match("/{$filter['value']}/i", $item[$key]);
      break;
    case 'not equal':
      return ($filter['value'] == $item[$key]);
      break;
    case 'equals':
    default:
      return ($filter['value'] == $item[$key]);
      break;
  }
}
