<?php
/**
 * @file
 */

/**
 * Displays the content type admin overview page.
 */
function imagex_aggregator_social_overview_types() {
  // TODO
  $types = imagex_aggregator_social_type_get_types();
  // TODO
  $names = imagex_aggregator_social_type_get_names();
  $field_ui = module_exists('field_ui');
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();

  foreach ($names as $key => $name) {
    $type = $types[$key];
    // TODO
    // if (imagex_aggregator_social_hook($type->type, 'form')) {
    if (TRUE) {
      $type_url_str = str_replace('_', '-', $type->type);
      $row = array(theme('imagex_aggregator_social_admin_overview', array('name' => $name, 'type' => $type)));
      // Set the edit column.
      $row[] = array('data' => l(t('edit'), 'admin/structure/imagex_aggregator_social_types/manage/' . $type_url_str));

      if ($field_ui) {
        // Manage fields.
        $row[] = array('data' => l(t('manage fields'), 'admin/structure/imagex_aggregator_social_types/manage/' . $type_url_str . '/fields'));

        // Display fields.
        $row[] = array('data' => l(t('manage display'), 'admin/structure/imagex_aggregator_social_types/manage/' . $type_url_str . '/display'));
      }

      // Set the delete column.
      if ($type->custom) {
        $row[] = array('data' => l(t('delete'), 'admin/structure/imagex_aggregator_social_types/manage/' . $type_url_str . '/delete'));
      }
      else {
        $row[] = array('data' => '');
      }

      $rows[] = $row;
    }
  }

  $build['imagex_aggregator_social_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No social types available. <a href="@link">Add social type</a>.', array('@link' => url('admin/structure/imagex_aggregator_social_types/add'))),
  );

  return $build;
}

/**
 * Generates the social type editing form.
 */
/*function imagex_aggregator_social_type_form($form, &$form_state, $imagex_aggregator_social_type = NULL) {
  $form['new'] = array(
    '#type' => 'value',
    '#value' => TRUE,
  );
  // If imagex_aggregator_social_type is null then load an empty one.
  if (is_null($imagex_aggregator_social_type)) {
    $plugin_info = _imagex_aggregator_social_default_plugin();
    $plugin_info['name'] = '';

    $imagex_aggregator_social_type = new socialCustom($plugin_info);
  }
  elseif (!method_exists($imagex_aggregator_social_type, 'getExportStatus') || $imagex_aggregator_social_type->getExportStatus() == 'Normal') {
    $form['new'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }

  // $disabled = !$imagex_aggregator_social_type->isEditable();
  $disabled = FALSE;

  if ($disabled) {
    drupal_set_message(t('This Social Media Type can not be edited'));
  }
  $form['imagex_aggregator_social_type'] = array(
    '#type' => 'value',
    '#value' => $imagex_aggregator_social_type,
  );

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $imagex_aggregator_social_type->getLabel(),
    '#description' => t('The human-readable name of this social media type.'),
    '#required' => TRUE,
    '#size' => 30,
    '#disabled' => $disabled,
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $imagex_aggregator_social_type->getDescription(),
    '#description' => t('The description of this social media type.'),
    '#disabled' => $disabled,
  );

  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($imagex_aggregator_social_type->type) ? $imagex_aggregator_social_type->type : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'imagex_aggregator_social_type_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this social media type. It must only contain lowercase letters, numbers, and underscores.'),
    '#disabled' => $disabled,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Social Media type'),
    '#weight' => 40,
    '#disabled' => $disabled,
  );

  // This is a new social type.
  if (!empty($imagex_aggregator_social_type->type)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#name' => 'delete',
      '#value' => t('Delete Social Media type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#disabled' => $disabled,
    );

    if (method_exists($imagex_aggregator_social_type, 'getExportStatus')) {
      if ($imagex_aggregator_social_type->getExportStatus() == 'Overridden') {
        $form['actions']['delete']['#name'] = 'revert';
        $form['actions']['delete']['#value'] = t('Revert to defaults');
      }
      else {
        unset($form['actions']['delete']);
      }

    }
  }
  return $form;
}*/

/**
 * Form API submit callback for the type form.
 */
/*function imagex_aggregator_social_type_form_submit(&$form, &$form_state) {
  $op = $form_state['clicked_button']['#name'];
  if (in_array($op, array('revert', 'delete'))) {
    $form_state['redirect'] = 'admin/structure/social-types/manage/' . $form['imagex_aggregator_social_type']['#value']->type . '/' . $op;
  }
  $imagex_aggregator_social_type = $form_state['values']['imagex_aggregator_social_type'];
  $imagex_aggregator_social_type->type = $form_state['values']['name'];
  $imagex_aggregator_social_type->setLabel($form_state['values']['label']);
  $imagex_aggregator_social_type->setDescription($form_state['values']['description']);
  $imagex_aggregator_social_type->save($form_state['values']['new']);
  $form_state['redirect'] = 'admin/structure/block-types';
  ctools_include('export');

  // Clear the site caches so that the new "manage fields" and "display fields"
  // pages for the new social type work.
  drupal_flush_all_caches();
}*/

/**
 * Returns HTML for a node type description for the content type admin overview page.
 *
 * @param $variables
 *   An associative array containing:
 *   - name: The human-readable name of the content type.
 *   - type: An object containing the 'type' (machine name) and 'description' of
 *     the content type.
 *
 * @ingroup themeable
 */
function theme_imagex_aggregator_social_admin_overview($variables) {
  $name = $variables['name'];
  $type = $variables['type'];
  //dpm($name);

  $output = check_plain($name);
  $output .= ' <small>' . t('(Machine name: @type)', array('@type' => $type->type)) . '</small>';
  $output .= '<div class="description">' . filter_xss_admin($type->description) . '</div>';
  return $output;
}
