(function ($, Drupal, window, document, undefined) {jQuery(document).ready(function(){
      // When the 'from date' is changed on the following date fields, set
      // the 'to date' equal to the 'from date'.
      var date_fields_default_to_from = [
        {
          field_name: 'field_event_dates',
          cardinality: 1
        }
      ];
      var css_selector = '';
      $.each(date_fields_default_to_from, function(index, field){
          for (var delta = 0; delta < field.cardinality; delta++) {
            css_selector += '#edit-' + field.field_name.replace(/_/g,'-') + '-' + delta + '-value-datepicker-popup-0, ';  
          }
      });
      css_selector = css_selector.substring(0, css_selector.length - 2); // Remove last ', '
      $(css_selector).change(function(){
          $('#' + $(this).attr('id').replace(/value/g, 'value2')).val($(this).val());
      });
});})(jQuery, Drupal, this, this.document);

