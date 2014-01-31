<?php

/**
 * @file
 * Social editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class ImagexAggregatorSocialUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {

    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Social Media',
      'description' => 'Add edit and update social media.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );

    // Change the overview menu type for the list of socials.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add a social media item',
      'description' => 'Add a new social media item',
      'page callback'  => 'imagex_aggregator_social_add_page',
      'access callback'  => 'imagex_aggregator_social_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'imagex_aggregator_social.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'

    );
    $items['admin/content/imagex_aggregator_socials/add'] = array(
      'title' => 'Add a social media item',
      'description' => 'Add a new social media item',
      'page callback'  => 'imagex_aggregator_social_add_page',
      'access callback'  => 'imagex_aggregator_social_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'imagex_aggregator_social.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'

    );

    // Add menu items to add each different type of entity.
    foreach (imagex_aggregator_social_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'imagex_aggregator_social_form_wrapper',
        'page arguments' => array(imagex_aggregator_social_create(array('imagex_aggregator_social_type' => $type->type))),
        'access callback' => 'imagex_aggregator_social_access',
        'access arguments' => array('edit', $type->type),
        'file' => 'imagex_aggregator_social.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
      );
      $items['admin/content/imagex_aggregator_socials/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'imagex_aggregator_social_form_wrapper',
        'page arguments' => array(imagex_aggregator_social_create(array('imagex_aggregator_social_type' => $type->type))),
        'access callback' => 'imagex_aggregator_social_access',
        'access arguments' => array('edit', $type->type),
        'file' => 'imagex_aggregator_social.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
      );
    }

    // Loading and editing social entities
    /*$items[$this->path . '/' . $wildcard] = array(
      'page callback' => 'imagex_aggregator_social_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'imagex_aggregator_social_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'imagex_aggregator_social.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );
    $items[$this->path . '/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );*/



    // Menu item for viewing socials
    $items['imagex_aggregator_social/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'imagex_aggregator_social_title',
      'title arguments' => array(1),
      'page callback' => 'imagex_aggregator_social_view',
      'page arguments' => array(1),
      'access callback' => 'imagex_aggregator_social_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    $items['imagex_aggregator_social/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );
    $items['imagex_aggregator_social/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'page callback' => 'imagex_aggregator_social_form_wrapper',
      'page arguments' => array(1),
      'access callback' => 'imagex_aggregator_social_access',
      'access arguments' => array('edit', 1),
      'weight' => 0,
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'imagex_aggregator_social.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );
    $items[$this->path . '/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'imagex_aggregator_social_delete_form_wrapper',
      'page arguments' => array(1),
      'access callback' => 'imagex_aggregator_social_access',
      'access arguments' => array('edit', 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'imagex_aggregator_social.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );

    return $items;

  }


  /**
   * Create the markup for the add Social Entities page within the class
   * so it can easily be extended/overriden.
   */
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }

    return theme('imagex_aggregator_social_add_list', array('content' => $content));
  }

}


/**
 * Form callback wrapper: create or edit a social.
 *
 * @param $social
 *   The social object being edited by this form.
 *
 * @see imagex_aggregator_social_edit_form()
 */
function imagex_aggregator_social_form_wrapper($social) {
  // Add the breadcrumb for the form's location.
  imagex_aggregator_social_set_breadcrumb();
  return drupal_get_form('imagex_aggregator_social_edit_form', $social);
}


/**
 * Form callback wrapper: delete a social.
 *
 * @param $social
 *   The social object being edited by this form.
 *
 * @see imagex_aggregator_social_edit_form()
 */
function imagex_aggregator_social_delete_form_wrapper($social) {
  // Add the breadcrumb for the form's location.
  //imagex_aggregator_social_set_breadcrumb();
  return drupal_get_form('imagex_aggregator_social_delete_form', $social);
}


/**
 * Form callback: create or edit a social.
 *
 * @param $social
 *   The social object to edit or for a create form an empty social object
 *     with only a social type defined.
 */
function imagex_aggregator_social_edit_form($form, &$form_state, $social) {
  // Add the default field elements.
  $form['item_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Item ID'),
    '#required' => TRUE,
    '#default_value' => isset($social->iid) ? $social->iid : '',
  );

  $form['imagex_aggregator_social_entity'] = array(
    '#type' => 'value',
    '#value' => $social,
  );

  field_attach_form('imagex_aggregator_social', $social, $form, $form_state);

  // Add the field related form elements.
  $form_state['imagex_aggregator_social'] = $social;
  field_attach_form('imagex_aggregator_social', $social, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save social'),
    '#submit' => $submit + array('imagex_aggregator_social_edit_form_submit'),
  );

  if (!empty($social->iid)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete social item'),
      '#suffix' => l(t('Cancel'), 'admin/content/imagex_aggregator_socials'),
      '#submit' => $submit + array('imagex_aggregator_social_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'imagex_aggregator_social_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the social form
 */
function imagex_aggregator_social_edit_form_validate(&$form, &$form_state) {
  $social = $form_state['values']['imagex_aggregator_social_entity'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('imagex_aggregator_social', $social, $form, $form_state);
}


/**
 * Form API submit callback for the social form.
 *
 * @todo remove hard-coded link
 */
function imagex_aggregator_social_edit_form_submit(&$form, &$form_state) {

  // $social = entity_ui_controller('social')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the social and go back to the list of socials

  $social = $form_state['values']['imagex_aggregator_social_entity'];
  $social->iid = $form_state['values']['item_id'];
  field_attach_submit('imagex_aggregator_social', $social, $form, $form_state);
  imagex_aggregator_social_save($social);
  $form_state['redirect'] = 'imagex_aggregator_social/' . $social->smid;

}

/**
 * Form API submit callback for the delete button.
 *
 * @todo Remove hard-coded path
 */
function imagex_aggregator_social_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'imagex_aggregator_social/' . $form_state['values']['imagex_aggregator_social_entity']->smid . '/delete';
}


/**
 * Form callback: confirmation form for deleting a social.
 *
 * @param $social
 *   The social to delete
 *
 * @see confirm_form()
 */
function imagex_aggregator_social_delete_form($form, &$form_state, $social) {
  $form_state['imagex_aggregator_social'] = $social;

  $form['#submit'][] = 'imagex_aggregator_social_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete social %name?', array('%name' => $social->iid)),
    'admin/content/imagex_aggregator_socials/imagex_aggregator_social',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for imagex_aggregator_social_delete_form
 */
function imagex_aggregator_social_delete_form_submit($form, &$form_state) {
  $social = $form_state['imagex_aggregator_social'];

  imagex_aggregator_social_delete($social);

  drupal_set_message(t('The social %name has been deleted.', array('%name' => $social->iid)));
  watchdog('social', 'Deleted social %name.', array('%name' => $social->iid));

  $form_state['redirect'] = 'admin/content/imagex_aggregator_socials';
}



/**
 * Page to add Social Entities.
 *
 * @todo Pass this through a proper theme function
 */
function imagex_aggregator_social_add_page() {
  $controller = entity_ui_controller('imagex_aggregator_social');
  return $controller->addPage();
}


/**
 * Displays the list of available social types for social creation.
 *
 * @ingroup themeable
 */
function theme_imagex_aggregator_social_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="social-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer imagex_aggregator_social types')) {
      $output = '<p>' . t('Social Entities cannot be added because you have not created any social types yet. Go to the <a href="@create-social-type">social type creation page</a> to add a new social type.', array('@create-social-type' => url('admin/structure/imagex_aggregator_social_types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No social types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}

/**
 * Sets the breadcrumb for administrative social pages.
 */
function imagex_aggregator_social_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Socials'), 'admin/content/imagex_aggregator_socials'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



