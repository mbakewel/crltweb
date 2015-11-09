/*
 * BEGIN: CONSULATION NODES
 */

// Handle clicks on the consultation service provider find user widget.
function field_consultation_serv_provider_add_user() {
  var added_user = false;
  var new_user = $('#edit-field-consultation-serv-provider-users').val();
  var duplicate_user = false;
  $.each($('table#field_consultation_serv_provider_values tr input[type=text]'), function(index, text){
    var value = $(text).attr('value');
    if (value == '') {
      $(text).attr('value', new_user);
      added_user = true;
      return false;
    }
    else if (new_user == value) {
      duplicate_user = true;
      return false;
    }
  });
  if (!added_user && !duplicate_user) {
    alert("Click the 'Add another item' button first to make room for the user!");
  }
}

// This assembles a client object from the client input on the consultation node form.
function crlt_consultation_build_client_from_input() {
  try {
    var client = {};
    client.name = $('#crlt_consultation_client_name').val();
    client.mail = $('#crlt_consultation_client_mail').val();
    client.first_name = $('#crlt_consultation_client_first_name').val();
    client.last_name = $('#crlt_consultation_client_last_name').val();
    client.department = $('#crlt_consultation_client_department').val();
    client.school = $('#crlt_consultation_client_school').val();
    client.institution = $('#crlt_consultation_client_institution').val();
    client.rank = $('#crlt_consultation_client_rank').val();
    return client;
  }
  catch (error) { alert('crlt_consultation_build_client_from_input() - ' + error); }
}

// Adds a client from the widget to the user reference field.
function crlt_consulation_client_add() {
  try {
    var added_client = false;
    var new_client = $('#crlt_consultation_client_name').val();
    if (new_client == '') {
      alert('Enter a client uniqname first!');
      return false;
    }
    var duplicate_client = false;
    $.each($('table#field_client_values tr input[type=text]'), function(index, text){
      var value = $(text).attr('value');
      if (value == '') {
        $(text).attr('value', new_client);
        added_client = true;
        $('#crlt_consultation_client_output').html('Added client ' + new_client + ' to consultation!');
        crlt_consulation_client_clear();
        return false;
      }
      else if (new_client == value) {
        duplicate_client = true;
        return false;
      }
    });
    if (!added_client && !duplicate_client) {
      alert("Click the 'Add another item' button first to make room for the client!");
    }    
  }
  catch (error) { alert('crlt_consulation_client_add() - ' + error); }
}

// Clears all the input on the client form.
function crlt_consulation_client_clear() {
  try {
    $('#crlt_consultation_client_name').val('');
    $('#crlt_consultation_client_mail').val('');
    $('#crlt_consultation_client_first_name').val('');
    $('#crlt_consultation_client_last_name').val('');
    $('#crlt_consultation_client_department').val('');
    $('#crlt_consultation_client_school').val('');
    $('#crlt_consultation_client_institution').val('');
    $('#crlt_consultation_client_rank').val('');
    $('#crlt_consultation_client_output').html('');
    $('#crlt_consultation_client_list').empty();
  }
  catch (error) { alert('crlt_consulation_client_clear() - ' + error); }
}

// Handles click on the 'Search' button on the node form.
function crlt_consulation_client_search() {
  try {
    $('#crlt_consultation_client_output').html('Searching...');
    var list = $('#crlt_consultation_client_list');
    $(list).empty().hide();
    var client = crlt_consultation_build_client_from_input();
    $.ajax({
        type: 'POST',
        url: Drupal.settings.basePath + 'crlt/consultation/client/search',
        dataType: 'json',
        success:crlt_consulation_client_search_success, 
        data:crlt_consultation_client_generate_data_string(client)
    });
  }
  catch (error) { alert('crlt_consulation_client_search() - ' + error);}
}

/**
 * Ajax success callback that will display client search results.
 */
function crlt_consulation_client_search_success(data) {
  try {
    $('#crlt_consultation_client_output').html(data.message);
    // Any results?
    if (data.results) {
      var list = $('#crlt_consultation_client_list');
      $(list).show();
      $.each(data.results, function(index, client){
          var item = '';
          if (client.profile_values_profile_last_name_value) { item += client.profile_values_profile_last_name_value + ', '; }
          if (client.profile_values_profile_first_name_value) { item += client.profile_values_profile_first_name_value + ' - '; }
          item += '(';
          //if (client.uid) { item += client.uid + ' - '; }
          if (client.users_name) { item += "<a onclick='javascript:crlt_consultation_client_load(" + JSON.stringify(client) + ");'>" + client.users_name + '</a>'; }
          item += ') - ' ;
          if (client.profile_values_institution_value) { item += client.profile_values_institution_value + ' - '; }
          if (client.profile_values_unit_value) { item += client.profile_values_unit_value + ' - '; }
          if (client.profile_values_department_value) { item += '(' + client.profile_values_department_value + ') - '; }
          if (client.profile_values_Rank_value) { item += client.profile_values_Rank_value + ' - '; }
          $(list).append('<li>' + item + '</li>');
      });
    }
    else { $(list).hide(); }
  }
  catch (error) { alert('crlt_consulation_client_search_success - ' + error); }
}

// Handle click on the 'Save Client' button on the node form.
function crlt_consulation_client_save() {
  try {
    var client = crlt_consultation_build_client_from_input();
    if (client.name == '' || client.mail == '' || client.first_name == '' || client.last_name == '') {
      alert('You must enter a uniqname, e-mail address, first name and last name!');
      return;
    }
    $('#crlt_consultation_client_output').html('Saving client...');
    $.ajax({
        type: 'POST',
        url: Drupal.settings.basePath + 'crlt/consultation/client/save',
        dataType: 'json',
        success: function(data){
          $('#crlt_consultation_client_output').html(data.message);
        },
        data:crlt_consultation_client_generate_data_string(client)
    });
  }
  catch (error) { alert('crlt_consulation_client_widget_save() - ' + error); }
} 

/**
 * Given a client object, this assembles the data string to POST during an ajax call.
 */
function crlt_consultation_client_generate_data_string(client) {
  try {
    var data = '';
    $.each(client, function(index, value){
      if (value) {
        data += index + '=' + encodeURIComponent(value) + '&';
      }
    });
    return data;
  }
  catch (error) { alert('crlt_consultation_client_generate_data_string() - ' + error); }
}

/**
 * Given a client JSON object from a views result, this will load the client into the input fields.
 */
function crlt_consultation_client_load(client) {
  try {
    if (client.users_name) {
      $('#crlt_consultation_client_name').val(client.users_name);
    }
    if (client.users_mail) {
      $('#crlt_consultation_client_mail').val(client.users_mail);
    }
    if (client.profile_values_profile_first_name_value) {
      $('#crlt_consultation_client_first_name').val(client.profile_values_profile_first_name_value);
    }
    if (client.profile_values_profile_last_name_value) {
      $('#crlt_consultation_client_last_name').val(client.profile_values_profile_last_name_value);
    }
    if (client.profile_values_department_value) {
      $('#crlt_consultation_client_department').val(client.profile_values_department_value);
    }
    if (client.profile_values_unit_value) {
      $('#crlt_consultation_client_school').val(client.profile_values_unit_value);
    }
    if (client.profile_values_institution_value) {
      $('#crlt_consultation_client_institution').val(client.profile_values_institution_value);  
    }
    if (client.profile_values_Rank_value) {
      $('#crlt_consultation_client_rank').val(client.profile_values_Rank_value);
    }
    if (client.users_name) {
      $('#crlt_consultation_client_output').html('Loaded client ' + client.users_name + ' into form!');
    }
    return false;
  }
  catch (error) { alert('crlt_consultation_client_load() - ' + error); }
}

/*
 * END: CONSULATION NODES
 */

