<?php

include '../php/ovbox.inc';
{
  $sitecfg = get_properties('site','config');
  if( $sitecfg['forcehttps'] ){
    if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
      if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
        $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
        header( "Location: ".$actual_link );
        die();
      }
    }
  }
}

include '../php/rest.inc';
include '../php/user.inc';
include '../php/session.inc';

  
if( isset($_POST['claimdevid']) ){
  $msg = '';
  if( claim_device_id( $user, $_POST['claimdevid'], $msg ) ){
    header( "Location: /account.php" );
  }else{
    print_head( $user, $style, $urlgroup );
    echo '<div class="deverror">'.$msg.'</div>';
    $alink = 'https://' . $_SERVER['HTTP_HOST'];
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
      $alink = 'http://' . $_SERVER['HTTP_HOST'];
    //echo '<p><a href="'.$alink.'">Continue</a></p>' . "\n";
    echo '<p><a href="account.php">Continue</a></p>' . "\n";
    print_foot($style);
  }
  die();
}
if( isset($_POST['terminateaccount']) ){
  flock($fp_user, LOCK_EX );
  terminate_account( $user );
  session_unset();
  session_destroy();
  header( "Location: /" );
  die();
}
if( isset($_POST['contact']) ){
  submit_contact( $user, $_POST['contact'], $_POST['message'] );
}

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">User profile</span><div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div></div>';

$doc = new DOMDocument('1.0');
$root = $doc->appendChild($doc->createElement('div'));
$root->setAttribute('class','userarea');
{
  // personal info:
  $div = create_section($root,$doc,'Personal info');
  $div->appendChild($doc->createTextNode('e-mail address (only used for password recovery):'));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('type','email');
  $el->setAttribute('name','usermail');
  $el->setAttribute('id','usermail');
  $el->setAttribute('value',$userprop['mail']);
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('type','button');
  $el->setAttribute('value',' Save ');
  $el->setAttribute('onclick','rest_setval(\'usermail\',get_value_by_id(\'usermail\',\'\'));');
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('value','reset password');
  $el->setAttribute('type','button');
  $el->setAttribute('onclick','rest_setval_post_reload( \'mypwreset\',\'\');;');
  $supp = $div->appendChild($doc->createElement('div'));
  if( $userprop['subscription'] )
    $msg = 'As a permanent supporter you have premium access.';
  else if( $userprop['validsubscription'] )
    $msg = 'You have premium access until '.date('Y-m-d',floatval($userprop['subscriptionend'])).' (yyyy-mm-dd).';
  else
    $msg = 'You do not have premium access. Please consider a donation; your donation of '.$site['subscriptionrate'].' €/month allows us to rent a powerful server.';
  $supp->appendChild($doc->createTextNode($msg));
  if( !$userprop['subscription'] ){
    $form = $div->appendChild($doc->createElement('form'));
    $form->setAttribute('action','https://www.paypal.com/donate');
    $form->setAttribute('method','post');
    $form->setAttribute('target','_top');
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','hosted_button_id');
    $inp->setAttribute('value','7FETYJ93A7KWC');
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','item_name');
    $inp->setAttribute('value','House of Consort - '.$user);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','image');
    $inp->setAttribute('src','https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif');
    $inp->setAttribute('border','0');
    $inp->setAttribute('name','submit');
    $inp->setAttribute('title','PayPal - The safer, easier way to pay online!');
    $inp->setAttribute('alt','Donate with PayPal button');
    $inp = $form->appendChild($doc->createElement('img'));
    $inp->setAttribute('alt','');
    $inp->setAttribute('border','0');
    $inp->setAttribute('src','https://www.paypal.com/en_DE/i/scr/pixel.gif');
    $inp->setAttribute('width','1');
    $inp->setAttribute('height','1');
    $supp = $div->appendChild($doc->createElement('div'));
    $supp->setAttribute('class','foto');
    $supp->appendChild($doc->createTextNode('Donations are processed manually. After a donation, it may take a few days for us to update your account status. If you think we missed your donation, please use the contact form below and provide the date and amount of your donation.'));
  }
}
{
  // personal info:
  $usergroups = list_groups($user);
  if(!empty($usergroups) ){
    $div = create_section($root,$doc,'Group info');
    $grplist = $div->appendChild($doc->createElement('ul'));
    foreach( list_groups($user) as $grp ){
      $p1 = $grplist->appendChild($doc->createElement('li'));
      $p1->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($grp.': '));
      $grpusers = get_group_users( $grp );
      sort($grpusers);
      foreach( $grpusers as $us ){
        $p1->appendChild($doc->createTextNode($us.' '));
      }
    }
    // main group selector:
    $inp = $div->appendChild($doc->createElement('label'));
    $inp->appendChild($doc->createTextNode('Primary group: '));
    $inp = $div->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','rest_setval_reload(\'primarygroup\',event.target.value);');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- no group -- '));
    if( $userprop['maingroup'] == '' )
      $opt->setAttribute('selected','');
    foreach($usergroups as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $userprop['maingroup'] == $us )
        $opt->setAttribute('selected','');
    }
  }
}
{
  // direct device claiming
  $div = create_section($root,$doc,'Reclaim a device');
  $form = $div->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $p = $form->appendChild($doc->createElement('p'));
  $p->appendChild($doc->createTextNode('If you are missing your device but you know the MAC address or device ID, you may claim it manually. To do so, please power it on and claim it within five minutes using this form.'));
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','claimdevid');
  $el->appendChild($doc->createTextNode('MAC address / device ID:'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','claimdevid');
  $el->setAttribute('id','claimdevid');
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('Claim this device'));
}
{
  // account termination
  $div = create_section($root,$doc,'Account termination');
  $form = $div->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','terminateaccount');
  $el->setAttribute('id','terminateaccount');
  $el->setAttribute('type','checkbox');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','terminateaccount');
  $el->appendChild($doc->createTextNode('I understand that the termination of my account cannot be undone. It will also remove all device configurations linked to this account.'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('terminate account'));
}
{
  // contact form
  $div = create_section($root,$doc,'Contact form');
  $form = $div->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','contact');
  $el->appendChild($doc->createTextNode('Enter your comments or wishes here:'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('textarea'));
  $el->setAttribute('rows','5');
  $el->setAttribute('cols','60');
  $el->setAttribute('name','message');
  $el->setAttribute('required','');
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','contact');
  $el->appendChild($doc->createTextNode('If you would like to receive an answer, please indicate here how we can contact you (e.g. your e-mail address):'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','text');
  $el->setAttribute('name','contact');
  $el->setAttribute('value',$userprop['mail']);
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('Submit'));
}
echo $doc->saveHTML() . "\n";

print_foot($style);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
