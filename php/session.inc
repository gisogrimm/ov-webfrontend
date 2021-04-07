<?php

session_start();

if( !isset($_SESSION['user']) ){
  header( "Location: /login.php" );
  die();
}

$user = $_SESSION['user'];

// validate user against db:
flock($fp_user, LOCK_EX );
if( !in_array($user,list_users()) ){
  session_unset();
  session_destroy();
  header( "Location: ?grp=".grouphash($urlgroup) );
  die();
}
$userprop = get_properties( $user, 'user' );
$device = get_device( $user );
$owneddevices = array_keys(owned_devices($user));
if( !in_array($device,$owneddevices) )
  $device = '';

if ( empty( $device ) ) {
  $ndev = reset($owneddevices);
  if( $ndev ){
    select_userdev( $user, $ndev );
    header( "Refresh:0" );
  }
}


modify_user_prop( $user, 'access', time() );
if( isset($_POST['agreepriv']) )
    modify_user_prop( $user, 'agreedprivacy', true );
if( isset($_POST['agreeterms']) )
    modify_user_prop( $user, 'agreedterms', true );
flock($fp_user, LOCK_UN );
flock($fp_dev, LOCK_EX );
$devprop = get_properties( $device, 'device' );
flock($fp_dev, LOCK_UN );
if( empty($devprop['owner']) )
  $devprop['owner'] = $user;

// style settings:
$urlgroup = '';
if( isset($_GET['grp'] ) )
  $urlgroup = get_group_by_hash($_GET['grp']);
$style = '';
if( !empty($urlgroup) ){
  $gprop = get_properties( $urlgroup, 'group' );
  $style = $gprop['style'];
}

$usergroups = list_groups($user);
$maingroup = $userprop['maingroup'];
if( !in_array($maingroup,$usergroups) )
  $maingroup = '';
$style = '';
if( !empty($maingroup) ){
  $groupprop = get_properties( $maingroup, 'group' );
  $style = $groupprop['style'];
}

// in a current session the logout was clicked - destroy session:
if( isset($_POST['logout']) ){
  unset($_SESSION['user']);
  session_unset();
  session_destroy();
  header( "Location: ?grp=".grouphash($urlgroup) );
  die();
}

if( isset($_GET['logout']) ){
  unset($_SESSION['user']);
  session_unset();
  session_destroy();
  header( "Location: ?grp=".grouphash($urlgroup) );
  die();
}

$site = get_properties('site','config');

if( !$userprop['validpw'] ){
  print_head( $user, $style, $urlgroup );
  $doc = new DOMDocument('1.0');
  $form = $doc->appendChild($doc->createElement('div'));
  $form->setAttribute('class','devprop');
  $form->appendChild($doc->createTextNode('New password (minimum 6 characters, 1 letter, 1 number): '));
  $form->appendChild($doc->createElement('br'));
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('name','password');
  $inp->setAttribute('type','password');
  $inp->setAttribute('id','newpassword');
  $inp->setAttribute('required','');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','submit');
  $inp->setAttribute('value','Submit');
  $inp->setAttribute('onclick','rest_setval_post_reload( \'updatepassword\', get_value_by_id(\'newpassword\',\'\' ));');
  echo $doc->saveHTML();
  print_foot($style);
  die();
}

if( !($userprop['agreedterms'] && $userprop['agreedprivacy']) ){
  print_head( $user, $style, $urlgroup );
  $doc = new DOMDocument('1.0');
  $form = $doc->appendChild($doc->createElement('form'));
  $form->setAttribute('class','devprop');
  $form->setAttribute('method','POST');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('name','agreepriv');
  $inp->setAttribute('type','checkbox');
  $inp->setAttribute('required','');
  if( $userprop['agreedprivacy'] )
    $inp->setAttribute('checked','');
  $lab = $form->appendChild($doc->createElement('label'));
  $lab->setAttribute('for','agreepriv');
  $lab->appendChild($doc->createTextNode('I have read and accept the '));
  $a = $lab->appendChild($doc->createElement('a'));
  $a->setAttribute('target','blank');
  $a->setAttribute('href','privacy.php');
  $a->appendChild($doc->createTextNode('privacy policy'));
  $lab->appendChild($doc->createTextNode(' / '));
  $a = $lab->appendChild($doc->createElement('a'));
  $a->setAttribute('target','blank');
  $a->setAttribute('href','privacy.php');
  $a->appendChild($doc->createTextNode('Datenschutzerkärung'));
  $lab->appendChild($doc->createTextNode('.'));
  $form->appendChild($doc->createElement('br'));
  // terms:
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('name','agreeterms');
  $inp->setAttribute('type','checkbox');
  $inp->setAttribute('required','');
  if( $userprop['agreedterms'] )
    $inp->setAttribute('checked','');
  $lab = $form->appendChild($doc->createElement('label'));
  $lab->setAttribute('for','agreeterms');
  $lab->appendChild($doc->createTextNode('I have read and accept the '));
  $a = $lab->appendChild($doc->createElement('a'));
  $a->setAttribute('target','blank');
  $a->setAttribute('href','terms.php');
  $a->appendChild($doc->createTextNode('Terms of Service'));
  $lab->appendChild($doc->createTextNode('.'));
  $form->appendChild($doc->createElement('br'));
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','submit');
  $inp->setAttribute('value','Submit');
  echo $doc->saveHTML();
  print_foot($style);
  die();
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
