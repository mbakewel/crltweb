<?php

/**
 * CRLT Grant Migration form
 *
 * Handles SiteMaker CSV export upload for grant processing
 */
function crlt_grant_application_migration_form( &$form_state ) {
  // This is required to upload files.
  // enctype="multipart/form-data" required by browsers to handle files.
  $form = array(
    '#attributes' => array( 'enctype' => "multipart/form-data" ),
  );
  // SiteMaker export CSV
  $form['file'] = array(
    '#type' => 'file',
    '#title' => t( 'SiteMaker Export File' ),
    '#description' => t( 'Upload the CRLT Grant Application SiteMaker export CSV. Allowed extension: csv' ),
  );
  $form['do_purge'] = array(
    '#type' => 'checkbox',
    '#title' => t( 'Purge existing SiteMaker Grant Application records' ),
    '#description' => t( 'Check this box to purge (delete) all previously migrated SiteMaker Grant Application records. This cannot be undone. Any changes since initial migration will be lost. Failing to purge may result in duplicate records.' ),
    '#default_value' => 0, // Purge by default
  );
  // Submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save'
  );
  return $form;
}

/**
 * Form validation & file handling
 */
function crlt_grant_application_migration_form_validate( $form, &$form_state ) {
  $file = file_save_upload( 'file', array(
      'file_validate_extensions' => array( 'csv' ),
    ) );

  // If the file passed validation:
  if ( isset( $file->filename ) ) {
    // Move the file, into the Drupal file system
    if ( file_move( $file, $file->filename ) ) {
      // Update the new file location in the database.
      drupal_write_record( 'files', $file, 'fid' );
      // Save the file for use in the submit handler.
      $form_state['storage']['file'] = $file;
    }
    else {
      form_set_error( 'file', t( 'Failed to write the uploaded file the site\'s file folder.' ) );
    }
  }
  else {
    form_set_error( 'file', t( 'Invalid file, only files with the extension csv are allowed' ) );
  }
}

/**
 * Form submission. Process CSV file.
 */
function crlt_grant_application_migration_form_submit( $form, &$form_state ) {
  $file = $form_state['storage']['file'];
  // We are done with the file, remove it from storage.
  unset( $form_state['storage']['file'] );
  // Make the storage of the file temporary
  file_set_status( $file, FILE_STATUS_TEMPORARY );
  // Purge existing records, if requested
  if ( $form_state['values']['do_purge'] ) {
    _crlt_grant_application_migration_purge();
  }
  drupal_set_message( t( 'Starting SiteMaker export migration. Memory used: @memory', array( '@memory' => memory_get_usage() ) ) );
  // Load CSV into associative arrays
  // $sitemaker_records = csv_to_array( $file->filepath );
  // foreach ($sitemaker_records as $sitemaker_record) {
  // echo '<pre>';
  // _crlt_grant_application_migration_add_node( $sitemaker_record );
  // echo $sitemaker_record['sitemaker_id'] . ',' . $sitemaker_record['projecttitle'] . ',' . $sitemaker_record['year'] . ',' . memory_get_usage() . PHP_EOL;
  // flush();
  // if(!$sitemaker_record['sitemaker_id']) {
  //   break;
  // }
  // }
  ini_set( "auto_detect_line_endings", true );
  if ( !file_exists( $file->filepath ) || !is_readable( $file->filepath ) ) {
    // drupal_set_message( sprintf( 'UPLOADED FILE MISSING!' ) );
    return FALSE;
  }

  $header = NULL;
  $data = array();

  if ( ( $handle = fopen( $file->filepath, 'r' ) ) !== FALSE ) {
    // drupal_set_message( sprintf( 'UPLOADED FILE HANDLE OPEN!' ) );
    while ( ( $row = fgetcsv( $handle ) ) !== FALSE ) {
      // drupal_set_message( sprintf( 'UPLOADED FILE LINE READ!' ) );
      if ( !$header ) {
        // drupal_set_message( sprintf( 'UPLOADED FILE HEADER PROCESSED!' ) );
        $header = $row;
      } else {
        $sitemaker_record = array_combine( $header, $row );
        // drupal_set_message( sprintf( '<pre>!!!!!!!!!! %s</pre>', print_r( $row, TRUE ) ) );
        // drupal_set_message( sprintf( '<pre>********** %s</pre>', print_r( $sitemaker_record, TRUE ) ) );
        _crlt_grant_application_migration_add_node( $sitemaker_record );
        // drupal_set_message( sprintf( 'UPLOADED FILE ROW PROCESSED! %s', memory_get_usage() ) );
        // echo $sitemaker_record['sitemaker_id'] /* . ',' . $sitemaker_record['projecttitle'] . ',' . $sitemaker_record['year'] */ . ',' . memory_get_usage() . PHP_EOL;
        // flush();

        //         if (memory_get_usage() > 167772160-1024) {
        //           die();
        //         }

        $sitemaker_record = null;
      }
    }
    fclose( $handle );
    // drupal_set_message( sprintf( 'UPLOADED FILE HANDLE CLOSED!' ) );
  }

  // Set a response for the user.
  drupal_set_message( t( 'SiteMaker export file @file has been migrated. Memory used: @memory', array( '@file' => $file->filename, '@memory' => memory_get_usage() ) ) );
}

if ( !function_exists( 'csv_to_array' ) ) {
  /**
   * Convert a comma separated file into an associated array.
   * The first row should contain the array keys.
   *
   * Example:
   *
   * @param string  $filename  Path to the CSV file
   * @param string  $delimiter The separator used in the file
   * @return array
   * @link http://gist.github.com/385876
   * @author Jay Williams <http://myd3.com/>
   * @copyright Copyright (c) 2010, Jay Williams
   * @license http://www.opensource.org/licenses/mit-license.php MIT License
   */
  function csv_to_array( $filename='', $delimiter=',' ) {
    ini_set( "auto_detect_line_endings", true );
    if ( !file_exists( $filename ) || !is_readable( $filename ) )
      return FALSE;

    $header = NULL;
    $data = array();
    if ( ( $handle = fopen( $filename, 'r' ) ) !== FALSE ) {
      while ( ( $row = fgetcsv( $handle, 1000, $delimiter ) ) !== FALSE ) {
        if ( !$header )
          $header = $row;
        else
          $data[] = array_combine( $header, $row );
      }
      fclose( $handle );
    }
    return $data;
  }
}
