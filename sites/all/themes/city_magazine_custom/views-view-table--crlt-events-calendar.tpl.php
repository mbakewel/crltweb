<?php
/**
 * @file views-view-table.tpl.php
 * Template to display a view as a table.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $class: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * @ingroup views_templates
 */

//drupal_set_message('<pre>' . print_r($view->result, true) . '</pre>');
//drupal_set_message('<pre>' . print_r($view->crlt_event_series_bg_colors, true) . '</pre>');
//drupal_set_message('<pre>' . print_r($rows, true) . '</pre>');

?>
<table class="<?php print $class; ?>"<?php print $attributes; ?>>
  <?php if (!empty($title)) : ?>
    <caption><?php print $title; ?></caption>
  <?php endif; ?>
  <thead>
    <tr>
      <?php foreach ($header as $field => $label): ?>
        <th class="views-field views-field-<?php print $fields[$field]; ?>">
          <?php print $label; ?>
        </th>
      <?php endforeach; ?>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($rows as $count => $row):
      // Color the row's event title column  according to the event type.
      // This data is provided by our hook_views_pre_render() implementation.
      $style_attribute = '';
      if (isset($view->crlt_event_series_bg_colors[$count]) && $view->crlt_event_series_bg_colors[$count] != '') {
        $style_attribute = 'style="background-color: ' . $view->crlt_event_series_bg_colors[$count] . '"';
      }
      // Add a class name to specify the week number, so we can highlight weeks in the table.
      $week_number = intval(date('W', strtotime($view->result[$count]->node_data_field_event_dates_field_event_dates_value)));
      $week_class = "crlt_even_week";
      if ($week_number % 2 == 1) {
        $week_class = "crlt_odd_week";
      }
      $row_classes[$count][] = $week_class;
    ?>
      <tr class="<?php print implode(' ', $row_classes[$count]); ?>">
        <?php
          $column = 0;
          foreach ($row as $field => $content): ?>
          <td class="views-field views-field-<?php print $fields[$field]; ?>" <?php if ($field == 'title') { print $style_attribute; } ?>>
            <?php
              // If we're on the first column, and there are event flag icons to render, render them.
              if ($column == 0) {
                if (isset($view->crlt_event_flags[$count]) && !empty($view->crlt_event_flags[$count])) {
                  foreach($view->crlt_event_flags[$count] as $img_path) {
                    print theme('image', $img_path, null, null, array('class' => 'crlt_event_flag'));
                  }
                }
              }
              // Print the table's cell data.
              print $content;
              $column++;
            ?>
          </td>
        <?php endforeach; ?>
      </tr>
    <?php endforeach; ?>
  </tbody>
</table>

