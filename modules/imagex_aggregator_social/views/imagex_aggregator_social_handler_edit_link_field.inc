<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class imagex_aggregator_social_handler_edit_link_field extends imagex_aggregator_social_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['imagex_aggregator_social_type'] = 'imagex_aggregator_social_type';
  }


  function render($values) {
    $type = $values->{$this->aliases['imagex_aggregator_social_type']};

    //Creating a dummy social to check access against
    $dummy_social = (object) array('type' => $type);
    if (!social_access('edit', $dummy_social)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $smid = $values->{$this->aliases['smid']};

    return l($text, 'imagex_aggregator_social/' . $smid . '/edit');
  }
}
