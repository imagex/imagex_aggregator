<?php
/**
 * @file
 * Theme functions
 */

/**
 * Renders the Filters form as a table
 */
function theme_imagex_aggregator_filter_form($vars) {
  $form = $vars['form'];
  foreach (element_children($form) as $key) {
    // Determine which button we are showing
    $button = isset($form[$key]['add']) ? 'add' : 'remove';

    if ($button == 'add') {
      // Unset the title elements since we will use table headers for this
      unset($form[$key]['item_key']['#title']);
      unset($form[$key]['condition']['#title']);
      unset($form[$key]['value']['#title']);
      unset($form[$key][$button]['#title']);
    }
    $rows[] = array(
      drupal_render($form[$key]['item_key']),
      drupal_render($form[$key]['condition']),
      drupal_render($form[$key]['value']),
      drupal_render($form[$key][$button]),
    );
  }
  
  return theme('table', array(
    'rows' => $rows,
    'header' => array(
      t('Feed Key'),
      t('Condition'),
      t('Value'),
      t('Action'),
    ),
    'caption' => t('Filters for %url', array('%url' => $form['#url'])),
  )); 
}

/**
 * Renders the Mapping form as a table
 */
function theme_imagex_aggregator_mapping_form($vars) {
  $form = $vars['form'];
  foreach (element_children($form) as $key) {
    // Determine which button we are showing
    $button = isset($form[$key]['add']) ? 'add' : 'remove';

    if ($button == 'add'){
      // Unset the title elements since we will use table headers for this
      unset($form[$key]['item_key']['#title']);
      unset($form[$key]['field']['#title']);
      unset($form[$key][$button]['#title']);
    }
    $rows[] = array(
      drupal_render($form[$key]['item_key']),
      drupal_render($form[$key]['field']),
      drupal_render($form[$key][$button]),
    );
  }
  
  return theme('table', array(
    'rows' => $rows,
    'header' => array(
      t('Feed Key'),
      t('@entity type @bundle - Field', array('@entity' => $form['#entity_label'], '@bundle' => $form['#bundle_label'])),
      t('Action'),
    ),
    'caption' => t('Mappings for %url', array('%url' => $form['#url'])),
  )); 
}