<?php

/**
 * @file
 * Model type editing UI.
 */

/**
 * UI controller.
 */
class ImagexAggregatorSocialTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage social entity types, including adding and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Generates the social type editing form.
 */
function imagex_aggregator_social_type_form($form, &$form_state, $imagex_aggregator_social_type, $op = 'edit') {

  if ($op == 'clone') {
    $imagex_aggregator_social_type->label .= ' (cloned)';
    $imagex_aggregator_social_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $imagex_aggregator_social_type->label,
    '#description' => t('The human-readable name of this social type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($imagex_aggregator_social_type->type) ? $imagex_aggregator_social_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $imagex_aggregator_social_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'imagex_aggregator_social_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this social type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  /*$form['data']['#tree'] = TRUE;
  $form['data']['sample_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('An interesting social switch'),
    '#default_value' => !empty($imagex_aggregator_social_type->data['sample_data']),
  );*/

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save social type'),
    '#weight' => 40,
  );

  //Locking not supported yet
  /*if (!$imagex_aggregator_social_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete social type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('imagex_aggregator_social_type_form_submit_delete')
    );
  }*/
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function imagex_aggregator_social_type_form_submit(&$form, &$form_state) {
  $imagex_aggregator_social_type = entity_ui_form_submit_build_entity($form, $form_state);
  $imagex_aggregator_social_type->save();
  $form_state['redirect'] = 'admin/structure/imagex_aggregator_social_types';
}

/**
 * Form API submit callback for the delete button.
 */
function imagex_aggregator_social_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/imagex_aggregator_social_types/manage/' . $form_state['imagex_aggregator_social_type']->type . '/delete';
}
