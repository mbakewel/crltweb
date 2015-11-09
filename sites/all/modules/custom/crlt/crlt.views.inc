<?php

/**
 * Implements hook_views_query_alter().
 */
function crlt_views_query_alter(&$view, &$query) {


return;


  if ($view->name == 'crlt_events_calendar') {
    // For whatever reason, the crlt_events_calendar view will not add the
    // field_event_flag to the list of fields, even though we have it specified
    // in the Views UI, so we'll manually add it to the query here.
    // Grab a copy of the field_note table_queue and use it to assemble our needed table.
    $alias = 'node_data_field_event_flag';
    $query->table_queue[$alias] = array(
      'table' => $alias,
      'num' => 1,
      'alias' => $alias,
      'relationship' => 'node'
    );
    $query->table_queue[$alias]['join'] = new views_join;
    $query->table_queue[$alias]['join']->definition = array(
      'table' => 'content_field_event_flag',
      'left_field' => 'vid',
      'field' => 'vid',
      'left_table' => 'node'
    );
    $query->table_queue[$alias]['join']->extra_type = 'AND';
    $query->table_queue[$alias]['join']->table = 'content_field_event_flag';
    $query->table_queue[$alias]['join']->left_table = 'node';
    $query->table_queue[$alias]['join']->left_field = 'vid';
    $query->table_queue[$alias]['join']->field = 'vid';
    $query->table_queue[$alias]['join']->type = 'LEFT';
    $query->table_queue[$alias]['join']->adjusted = 1;
    $query->tables['node']['node_data_field_event_flag'] = array(
      'count' => 1,
      'alias' => 'node_data_field_event_flag'
    );
    $query->fields['node_data_field_event_flag_field_event_flag_value'] = array(
      'field' => 'field_event_flag_value',
      'table' => 'node_data_field_event_flag',
      'alias' => 'node_data_field_event_flag_field_event_flag_value'
    );
  }
}

