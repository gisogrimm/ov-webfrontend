<?php
include_once "mail.inc";

/*
 * This file is included from rest.php and user-pages
 */

function get_rooms_user( $user, $userprop, $usergroups, $devroom )
{
  $allrooms = list_db('room');
  ksort($allrooms);
  $drooms = array();
  $mgrooms = array();
  $grooms = array();
  $orooms = array();
  foreach( $allrooms as $room=>$rprop){
    if( ($rprop['age'] < 1000) &&
        ((!$rprop['private']) || ($rprop['owner']==$user)) &&
        ($userprop['seesall'] || ($rprop['owner']==$user) || in_array($rprop['group'],$usergroups)) &&
        (empty($rprop['group']) || (in_array($rprop['group'],$usergroups)))){
      $rprop['id'] = $room;
      $rprop['entered'] = $devroom == $room;
      $roomdev = get_devices_in_room( $room, false, true );
      ksort($roomdev);
      if( $rprop['lock'] && empty($roomdev) ){
        modify_room_prop( $room, 'lock', false );
        $rprop['lock'] = false;
      }
      $roomdevp = array();
      $rprop['roomdev'] = $roomdev;
      if( $rprop['entered'] ){
        $drooms[] = $rprop;
      }else{
        $rprop['lat'] = array();
        if( !empty($rprop['group']) ){
          if( $userprop['maingroup'] == $rprop['group'] )
            $mgrooms[] = $rprop;
          else
            $grooms[] = $rprop;
        }else
          $orooms[] = $rprop;
      }
    }
  }
  $allrooms = array();
  foreach( $drooms as $room )
    $allrooms[] = $room;
  foreach( $mgrooms as $room )
    $allrooms[] = $room;
  foreach( $grooms as $room )
    $allrooms[] = $room;
  foreach( $orooms as $room )
    $allrooms[] = $room;
  return $allrooms;
}

function set_getprop( &$prop, $key )
{
  if( isset($_GET[$key]) ){
    $prop[$key] = $_GET[$key];
  }
}

function set_getprop_float( &$prop, $key )
{
  if( isset($_GET[$key]) ){
    $prop[$key] = floatval($_GET[$key]);
  }
}

function update_pw( $password, $username, &$msg )
{
  if( empty($username) ){
    $msg = 'No user name was provided.';
    return false;
  }
  if( empty($password) ){
    $msg = 'No password was provided.';
    return false;
  }
  $errs = array();
  if( !checkPassword($password,$errs) ){
    foreach($errs as $err)
      $msg = $msg . '<br/>'.$err;
    return false;
  }
  $users = list_users();
  if( !in_array( $username, $users ) ){
    $msg = 'The user "'.$username.'" does not exist.';
    return false;
  }
  $pwhash = password_hash( $password, PASSWORD_DEFAULT );
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $pws[$username] = $pwhash;
  file_put_contents( $fname, json_encode( $pws ));
  modify_user_prop( $username, 'validpw', true);
  return true;
}

function checkPassword($pwd, &$errors) {
  $errors_init = $errors;
  if (strlen($pwd) < 6) {
    $errors[] = "Password too short!";
  }
  if (!preg_match("#[0-9]+#", $pwd)) {
    $errors[] = "Password must include at least one number!";
  }
  if (!preg_match("#[a-zA-Z]+#", $pwd)) {
    $errors[] = "Password must include at least one letter!";
  }     
  return ($errors == $errors_init);
}

function submit_contact( $user, $contact, $msg )
{
  $mailmsg = 'From: '.$user.' ('.$contact.")\n\n".$msg."\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  $sitecfg = get_properties('site','config');
  sendmail( $sitecfg['mail'], 'ovbox contact from '.$user, $mailmsg );
}

function set_getprop_post( &$prop, $key )
{
    if( isset($_POST[$key]) ){
        $prop[$key] = $_POST[$key];
    }
}

function set_getprop_post_float( &$prop, $key )
{
    if( isset($_POST[$key]) ){
        $prop[$key] = floatval($_POST[$key]);
    }
}


/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
