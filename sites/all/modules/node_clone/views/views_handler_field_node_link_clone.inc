<?php
/**
 * @file
 * Views field handler for Node_Clone module.
 */

/**
* Field handler to present a clone node link.
*
* Closely modeled after views/modules/node/views_handler_field_node_link_edit.inc
*/
class views_handler_field_node_link_clone extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['uid'] = 'uid';
    $this->additional_fields['type'] = 'type';
    $this->additional_fields['format'] = array('table' => 'node_revisions', 'field' => 'format');
  }

  /**
   * Determine if this field can allow advanced rendering.
   *
   * Fields can set this to FALSE if they do not wish to allow
   * token based rewriting or link-making.
   */
  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['destination'] = array('default' => 1, 'translatable' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['destination'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add destination query string to return to the View'),
      '#default_value' => $this->options['destination'],
    );
  }

  function render($values) {
    // Insure that user has access to clone this node.
    $node = new stdClass();
    $node->nid = $values->{$this->aliases['nid']};
    $node->uid = $values->{$this->aliases['uid']};
    $node->type = $values->{$this->aliases['type']};
    $node->format = $values->{$this->aliases['format']};
    $node->status = 1; // unpublished nodes ignore access control
    if (!clone_access_cloning($node)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('clone');
    $options = array();
    if ($this->options['destination']) {
      $options['query'] = drupal_get_destination();
    }
    return l($text, "node/$node->nid/clone", $options);
  }
}
