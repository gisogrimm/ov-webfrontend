<?php

/*
 * This file is included from user and admin pages
 */

function crypt_apr1_md5($plainpasswd, $salt)
{
  // APR1-MD5 encryption method (windows compatible)
  $tmp = "";
  $len = strlen($plainpasswd);
  $text = $plainpasswd.'$apr1$'.$salt;
  $bin = pack("H32", md5($plainpasswd.$salt.$plainpasswd));
  for($i = $len; $i > 0; $i -= 16) { $text .= substr($bin, 0, min(16, $i)); }
  for($i = $len; $i > 0; $i >>= 1) { $text .= ($i & 1) ? chr(0) : $plainpasswd{0}; }
  $bin = pack("H32", md5($text));
  for($i = 0; $i < 1000; $i++)
  {
    $new = ($i & 1) ? $plainpasswd : $bin;
    if ($i % 3) $new .= $salt;
    if ($i % 7) $new .= $plainpasswd;
    $new .= ($i & 1) ? $bin : $plainpasswd;
    $bin = pack("H32", md5($new));
  }
  for ($i = 0; $i < 5; $i++)
  {
    $k = $i + 6;
    $j = $i + 12;
    if ($j == 16) $j = 5;
    $tmp = $bin[$i].$bin[$k].$bin[$j].$tmp;
  }
  $tmp = chr(0).chr(0).$bin[11].$tmp;
  $tmp = strtr(strrev(substr(base64_encode($tmp), 2)),
               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
               "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  return "$"."apr1"."$".$salt."$".$tmp;
}

function htpw_matches($password, $filePasswd)
{
  if (strpos($filePasswd, '$apr1') === 0)
  {
    // MD5
    $passParts = explode('$', $filePasswd);
    $salt = $passParts[2];
    $hashed = crypt_apr1_md5($password, $salt);
    return $hashed == $filePasswd;
  }
  elseif (strpos($filePasswd, '{SHA}') === 0)
  {
    // SHA1
    $hashed = "{SHA}" . base64_encode(sha1($password, TRUE));
    return $hashed == $filePasswd;
  }
  elseif (strpos($filePasswd, '$2y$') === 0)
  {
    // Bcrypt
    return password_verify ($password, $filePasswd);
  }
  else
  {
    // Crypt
    $salt = substr($filePasswd, 0, 2);
    $hashed = crypt($password, $salt);
    return $hashed == $filePasswd;
  }
  return false;
}

function html_device_selector( $user, $currentdevice )
{
  $dprop = get_properties($currentdevice,'device');
  $devs = list_unclaimed_devices();
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $div = $root->appendChild($doc->createElement('div'));
  $div->setAttribute('id','deverror');
  $div->setAttribute('class','deverror');
  $div->setAttribute('style','display: none;');
  if( (!empty($dprop['message']))&&($dprop['age']<3600) ){
    $div->setAttribute('style','display: block;');
    $b = $div->appendChild($doc->createElement('b'));
    $b->appendChild($doc->createTextNode('Device error:'));
    $div->appendChild($doc->createElement('br'));
    $div->appendChild($doc->createTextNode($dprop['message']));
    if( strpos($dprop['message'],'Unable to connect to the JACK server')!==false){
      $div->appendChild($doc->createElement('br'));
      $div->appendChild($doc->createTextNode('Is your sound card connected and configured correctly?'));
    }
  }
  $div = $root->appendChild($doc->createElement('div'));
  $select = $div->appendChild($doc->createElement('select'));
  $select->setAttribute('id','deviceselector');
  $select->setAttribute('class','uibutton');
  $select->setAttribute('oninput','location.href=\'?devselect=\'+event.target.value;');
  $select->appendChild($doc->createElement('option'))->appendChild($doc->createTextNode(' -- please select a device -- '));
  foreach( owned_devices( $user ) as $dev=>$sdprop ){
    $opt = $select->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$dev);
    if( $currentdevice == $dev )
      $opt->setAttribute('selected','');
    $active = '';
    if( $sdprop['age'] < 20 )
      $active = ' *active*';
    $opt->appendChild($doc->createTextNode($dev.' ('.$sdprop['label'].')'.$active));
  }
  $pres = $div->appendChild($doc->createElement('span'));
  if(!empty($dprop['preset'])){
    $pres->appendChild($doc->createTextNode($dprop['preset']));
    $pres->setAttribute('class','presetspan presetact');
  }
  $inp = $div->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplay(\'devsettings\',\'device settings\');');
  $inp->setAttribute('value','show device settings');
  echo $doc->saveHTML();
}

function create_inputportcfg( &$doc, &$form, $devprop )
{
  // store input channel configuration:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','hidden');
  $el->setAttribute('id','jsinputchannels');
  $el->setAttribute('name','jsinputchannels');
  $el->setAttribute('value',json_encode($devprop['inputchannels']));
  // pass hw channel list to javascript:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','hidden');
  $el->setAttribute('id','hwinputchannels');
  $el->setAttribute('value',json_encode($devprop['hwinputchannels']));
  $div = $form->appendChild($doc->createElement('div'));
  $div->setAttribute('id','jsinputchannelsdiv');
  // load java scripts:
  $script = $form->appendChild($doc->createElement('script'));
  $script->appendChild($doc->createTextNode(file_get_contents( '../js/inputchannels.js' )));
}

function add_input_element( &$parent, &$doc, &$devprop, $prop, $type, $label, $endbr = true )
{
  $el = $parent->appendChild($doc->createElement('label'));
  $el->setAttribute('for',$prop);
  $el->appendChild($doc->createTextNode($label));
  $parent->appendChild($doc->createElement('br'));
  $el = $parent->appendChild($doc->createElement('input'));
  $el->setAttribute('id',$prop);
  $el->setAttribute('name',$prop);
  $el->setAttribute('type',$type);
  $el->setAttribute('value',$devprop[$prop]);
  if( $endbr )
    $parent->appendChild($doc->createElement('br'));
  return $el;
}

function add_expert_div( &$el, &$doc )
{
  $div = $el->appendChild( $doc->createElement('div') );
  //$div->setAttribute('id','expert');
  $div->setAttribute('class','expert');
  $div->setAttribute('style','display: none;');
  return $div;
}

function set_dev_room_pos( $room )
{
  $roomdev = get_devices_in_room( $room, true );
  ksort($roomdev);
  $stagewidth = 360;
  $az = 0;
  $daz = $stagewidth/count($roomdev)*(pi()/180);
  $az = $az*(pi()/180)-0.5*$daz;
  $radius = 1.2;
  foreach( $roomdev as $chair => $dev ){
    $az = $az+$daz;
    $pos_x = $radius * cos( -$az );
    $pos_y = -$radius * sin( -$az );
    $dprop = get_properties( $dev, 'device' );
    $dprop['position'] = array('x'=>$pos_x,'y'=>$pos_y,'z'=>0);
    $dprop['orientation'] = array('z'=>(180/pi()*($az+pi())),'y'=>0,'x'=>0);
    set_properties( $dev, 'device', $dprop );
  }
}

function select_userdev( $user, $dev )
{
  $fname = '../db/' . $user . '.userdevice';
  if( empty($dev) ){
    unlink($fname);
  }else{
    file_put_contents( $fname, $dev );
  }
}

function room_swap_devices( $dev1, $dev2 )
{
  $dprop = get_properties( $dev1, 'device' );
  $room = $dprop['room'];
  $members = get_devices_in_room( $room );
  $chair1 = -1;
  $chair2 = -1;
  foreach( $members as $key => $dev ){
    if( $dev == $dev1 )
      $chair1 = $key;
    if( $dev == $dev2 )
      $chair2 = $key;
  }
  if( ($chair1>=0) && ($chair2>=0) ){
    modify_device_prop( $dev1, 'chair', $chair2 );
    modify_device_prop( $dev2, 'chair', $chair1 );
    set_dev_room_pos( $room );
  }
}

function lock_room( $room, $device, $lock )
{
  if( in_array($device, get_devices_in_room( $room ) ) ){
    modify_room_prop( $room, 'lock', $lock );
  }
}

function html_show_user( $user, $device, $devprop )
{
  $uprop = get_properties( $user, 'user' );
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->setAttribute('class','userarea');
  $p = $root->appendChild($doc->createElement('p'));
  $devspan = $p->appendChild($doc->createElement('span'));
  $devspan->setAttribute('id','deviceuser');
  $devspan->appendChild($doc->createTextNode('You are logged in as user '));
  $devspan->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($user));
  if( empty( $device ) ){
    $devspan->appendChild($doc->createTextNode(' with no device.'));
  }else{
    $class = 'psvmember';
    $state = '';
    $lastseen = '';
    $otherdev = '';
    if( $devprop['age'] < 20 ){
      $class = 'actmember';
      $state = 'active';
    }else{
      $lastseen = ', inactive since '.numage2str($devprop['age']);
      $oact = false;
      foreach( owned_devices($user) as $od=>$odp )
        if( $odp['age']<20 )
          $oact = true;
      if( $oact )
        $otherdev = ' You own active devices - please check the device selector below to access them.';
    }
    $devspan->appendChild($doc->createTextNode(' with '.$state.' device '));
    $span = $devspan->appendChild($doc->createElement('span'));
    $span->setAttribute('class',$class);
    $span->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($device.' ('.$devprop['label'].')' ));
    $devspan->appendChild($doc->createTextNode($lastseen.'.'.$otherdev));
    if( ($devprop['age'] < 20) && (($devprop['bandwidth']['tx']>0)||($devprop['bandwidth']['rx']>0)) ){
      if( $devprop['bandwidth']['tx'] >= 100000 )
        $txstr = number_format(0.000001*$devprop['bandwidth']['tx'],2).' MBps';
      else
        $txstr = number_format(0.001*$devprop['bandwidth']['tx'],2).' kBps';
      if( $devprop['bandwidth']['rx'] >= 100000 )
        $rxstr = number_format(0.000001*$devprop['bandwidth']['rx'],2).' MBps';
      else
        $rxstr = number_format(0.001*$devprop['bandwidth']['rx'],2).' kBps';
      $devspan->appendChild($doc->createTextNode(' sending: '. $txstr.', receiving: '. $rxstr));
    }
    if( ($devprop['age'] < 20) && ($devprop['cpuload']>0) ){
      $devspan->appendChild($doc->createTextNode(' CPU load: '. number_format(100*$devprop['cpuload'],1).'%'));
    }
  }
  $inf = $root->appendChild($doc->createElement('div'));
  $inf->setAttribute('id','groupinfo');
  $inf->setAttribute('class','devprop');
  $inf->setAttribute('style','display: none;');
  $form = $inf->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','usermail');
  $el->appendChild($doc->createTextNode('e-mail address (only used for password recovery):'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','email');
  $el->setAttribute('name','usermail');
  $el->setAttribute('value',$uprop['mail']);
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('Save'));
  $inf->appendChild($doc->createElement('hr'));
  $el = $inf->appendChild($doc->createElement('input'));
  $el->setAttribute('value','reset password');
  $el->setAttribute('type','button');
  $el->setAttribute('onclick','location.href=\'?mypwreset\';');
  $inf->appendChild($doc->createElement('hr'));
  $el = $inf->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('You are a member of these groups:'));
  foreach( list_groups($user) as $grp ){
    $p1 = $inf->appendChild($doc->createElement('div'));
    $p1->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode($grp.': '));
    $grpusers = get_group_users( $grp );
    sort($grpusers);
    foreach( $grpusers as $us ){
      $p1->appendChild($doc->createTextNode($us.' '));
    }
  }
  $inf->appendChild($doc->createElement('hr'));
  // main group selector:
  $inp = $inf->appendChild($doc->createElement('label'));
  $inp->appendChild($doc->createTextNode('Primary group: '));
  $inp = $inf->appendChild($doc->createElement('select'));
  $inp->setAttribute('oninput','location.href=\'?setmaingroup=\'+event.target.value;');
  $opt = $inp->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','');
  $opt->appendChild($doc->createTextNode(' -- no group -- '));
  if( $uprop['maingroup'] == '' )
    $opt->setAttribute('selected','');
  foreach(list_groups($user) as $us){
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$us);
    $opt->appendChild($doc->createTextNode($us));
    if( $uprop['maingroup'] == $us )
      $opt->setAttribute('selected','');
  }
  $inp = $p->appendChild($doc->createElement('input'));
  // manual device claiming:
  $inf->appendChild($doc->createElement('hr'));
  $form = $inf->appendChild($doc->createElement('form'));
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
  //
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplay(\'groupinfo\',\'user profile\');');
  $inp->setAttribute('value','show user profile');
  echo $doc->saveHTML() . "\n";
}

function html_show_device( $user, $device, $devprop )
{
  html_device_selector( $user, $device );
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $devset = $root->appendChild($doc->createElement('div'));
  $devset->setAttribute('class','devprop');
  $devset->setAttribute('id','devsettings');
  $devset->setAttribute('style','display: none;');
  // preset list:
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device setting presets:'));
  $presets = get_properties( $device, 'devpresets' );
  unset($presets['now']);
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','presetarea');
  if( !empty($presets) ){
    $presets = array_keys($presets);
    foreach( $presets as $preset ){
      $xclass = '';
      if( $preset == $devprop['preset'] )
        $xclass = ' presetact';
      $span = $el->appendChild($doc->createElement('span'));
      $span->setAttribute('class','presetspan'.$xclass);
      $inp = $span->appendChild($doc->createElement('input'));
      $inp->setAttribute('class','presetload'.$xclass);
      $inp->setAttribute('type','button');
      $inp->setAttribute('value',$preset);
      $inp->setAttribute('onclick','load_preset(this.value);');
      $inp = $span->appendChild($doc->createElement('input'));
      $inp->setAttribute('class','presetrm');
      $inp->setAttribute('type','button');
      $inp->setAttribute('value','X');
      $inp->setAttribute('name',$preset);
      $inp->setAttribute('onclick','rm_preset(this.name);');
    }
  }
  // settings presets:
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','presetspan');
  $inp = $span->appendChild($doc->createElement('input'));
  $inp->setAttribute('id','savepresetname');
  //$inp->setAttribute('class','presetspan');
  $inp->setAttribute('placeholder','Save current settings as preset');
  $inp->setAttribute('onchange','create_preset();');
  //$inp = $span->appendChild($doc->createElement('input'));
  //$inp->setAttribute('type','button');
  //$inp->setAttribute('value','Create new');
  //$inp->setAttribute('onclick','create_preset();');
  $devset->appendChild($doc->createElement('hr'));
  // device settings:
  $form = $devset->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','toggledisplayclass(\'expert\',\'expert settings\');');
  $inp->setAttribute('value','show expert settings');
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // device properties:
  // label:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','label');
  $el->appendChild($doc->createTextNode('device label (appears in rooms and the mixer of the others): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('id','label');
  $el->setAttribute('name','label');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[a-zA-Z0-9-_]*');
  $el->setAttribute('value',$devprop['label']);
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $form->appendChild($doc->createElement('br'));
  $form->appendChild($doc->createElement('hr'));
  // sound card settings:
  $div = $form->appendChild($doc->createElement('div'));
  $el = $div->appendChild($doc->createElement('div'));
  $div->setAttribute('id','sndcardsettings');
  $div->appendChild($doc->createTextNode('Sound card:'));
  $div->appendChild($doc->createElement('br'));
  // jack device:
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('id','jackdevice');
  $el->setAttribute('name','jackdevice');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  //
  $alsadevs = array('highest'=>'use highest device number','manual'=>'jack is started manually','hw:1'=>'device 1 (typically first USB device)');
  if( is_array($devprop['alsadevs']) )
    $alsadevs = array_merge( $alsadevs, $devprop['alsadevs']);
  foreach( $alsadevs as $adev=>$desc ){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$adev);
    $opt->appendChild($doc->createTextNode($desc . ' ('.$adev.')'));
    if( $devprop['jackdevice'] == $adev )
      $opt->setAttribute('selected','');
  }
  // use plughw device:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('type','checkbox');
  $el->setAttribute('name','jackplugdev');
  $el->setAttribute('id','jackplugdev');
  $el->setAttribute('value','jackplugdev');
  $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  if( $devprop['jackplugdev'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
  $el->setAttribute('for','jackplugdev');
  $el->appendChild($doc->createTextNode('use plugin device layer'));
  $div->appendChild($doc->createElement('br'));
  // end of device.
  $el = add_input_element( $div, $doc, $devprop, 'jackrate', 'number','Sampling rate in Hz: ',false);
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('onchange','update_jack_rate( this.value );');
  $opts = array('16000','22050','24000','32000','44100','48000');
  foreach($opts as $o){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$o);
    if( $o == $devprop['jackrate'] )
      $opt->setAttribute('selected','');
    $opt->appendChild($doc->createTextNode($o));
  }
  $div->appendChild($doc->createElement('br'));
  $el = add_input_element( $div, $doc, $devprop, 'jackperiod', 'number','Period size in samples (typically 2ms, i.e. 96 for 48000 Hz Sampling rate): ');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $divex = add_expert_div($form,$doc);
  $el = add_input_element( $divex, $doc, $devprop, 'jackbuffers', 'number','Number of buffers (typically 2): ');
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('min',2);
  if( version_compare("ovclient-0.5.11-8cc47fd",$devprop['version'])<0 ){
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('value','restart audio system');
    $el->setAttribute('type','button');
    $el->setAttribute('onclick','location.href=\'?jackrestart='.urlencode($device).'\';');
  }
  $form->appendChild($doc->createElement('hr'));
  // Audio settings:
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Audio settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // input port:
  create_inputportcfg( $doc, $form, $devprop );
  // output port:
  $div = add_expert_div($form,$doc);
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','outputport');
  $el->appendChild($doc->createTextNode('output ports (to which your headphones are connected): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','outputport1');
  $el->setAttribute('name','outputport1');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['outputport1']);
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','outputport2');
  $el->setAttribute('name','outputport2');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['outputport2']);
  $div->appendChild($doc->createElement('br'));
  // extra ports:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','xport');
  $el->appendChild($doc->createTextNode('extra ports (json expression, e.g., {"Giso:out_1":"ardour:Giso/in"}): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('size','40');
  $el->setAttribute('id','xport');
  $el->setAttribute('name','xport');
  $el->setAttribute('type','text');
  $el->setAttribute('value',$devprop['xport']);
  $div->appendChild($doc->createElement('br'));
  // receiver type:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','rectype');
  $el->appendChild($doc->createTextNode('receiver type: '));
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','rectype');
  $el->setAttribute('name','rectype');
  $recdesc = array('ortf'=>'Commonly used stereo microphone technique','hrtf'=>'Binaural Head Related Transfer Function simulation','itu51'=>'ITU 5.1 rendering, channel order L,R,C,LFE,Ls,Rs','omni'=>'mono');
  foreach( $recdesc as $rectype=>$desc ){
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value',$rectype);
    if( $devprop['rectype'] == $rectype )
      $opt->setAttribute('selected','');
    $opt->appendChild($doc->createTextNode($rectype.': '.$desc));
  }
  $div->appendChild($doc->createElement('br'));
  // playback gain:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','playbackgain');
  $el->appendChild($doc->createTextNode('playback gain in dB (equivalent to changing the input gain): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','playbackgain');
  $el->setAttribute('name','playbackgain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['playbackgain']);
  $form->appendChild($doc->createElement('br'));
  // master gain:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','playbackgain');
  $el->appendChild($doc->createTextNode('master gain in dB (equivalent to changing the headphone gain): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','mastergain');
  $el->setAttribute('name','mastergain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['mastergain']);
  $form->appendChild($doc->createElement('br'));
  // ego monitor:
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','egogain');
  $el->appendChild($doc->createTextNode('ego monitor gain in dB (how much of your own microphone is added to your headphone): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','egogain');
  $el->setAttribute('name','egogain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-30');
  $el->setAttribute('max','10');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['egogain']);
  $lab = $form->appendChild($doc->createElement('label'));
  $lab->setAttribute('class','switch');
  $el = $lab->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','selfmonitor');
  $el->setAttribute('name','selfmonitor');
  $el->setAttribute('type','checkbox');
  if( $devprop['selfmonitor'] )
    $el->setAttribute('checked','');
  $el = $lab->appendChild($doc->createElement('span'));
  $el->setAttribute('class','slider round');
  $el->appendChild($doc->createTextNode(''));
  $form->appendChild($doc->createElement('br'));
  //
  $div = add_expert_div($form,$doc);
  // ego monitor delay:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','selfmonitordelay');
  $el->appendChild($doc->createTextNode('self monitor delay in milliseconds: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','selfmonitordelay');
  $el->setAttribute('name','selfmonitordelay');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','1000');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['selfmonitordelay']);
  $div->appendChild($doc->createElement('br'));
  // reverb:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','reverb');
  $el->setAttribute('id','reverb');
  $el->setAttribute('type','checkbox');
  if( $devprop['reverb'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','reverb');
  $el->appendChild($doc->createTextNode('render reverb '));
  $div->appendChild($doc->createElement('br'));
  // ism
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','renderism');
  $el->setAttribute('id','renderism');
  $el->setAttribute('type','checkbox');
  if( $devprop['renderism'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','renderism');
  $el->appendChild($doc->createTextNode('render shoebox ISM '));
  $div->appendChild($doc->createElement('br'));
  // reverb gain:
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','devrvbgain');
  $el->appendChild($doc->createTextNode('extra reverb gain in dB: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','devrvbgain');
  $el->setAttribute('name','rvbgain');
  $el->setAttribute('type','number');
  $el->setAttribute('min','-20');
  $el->setAttribute('max','20');
  $el->setAttribute('step','0.1');
  $el->setAttribute('value',$devprop['rvbgain']);
  $div->appendChild($doc->createElement('br'));
  // raw mode:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','rawmode');
  $el->setAttribute('id','rawmode');
  $el->setAttribute('type','checkbox');
  if( $devprop['rawmode'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','rawmode');
  $el->appendChild($doc->createTextNode('raw mode - no virtual acoustics '));
  // head tracking:
  $div = add_expert_div($form,$doc);
  $el = $div->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Head tracking:'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtracking');
  $el->setAttribute('id','headtracking');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtracking'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtracking');
  $el->appendChild($doc->createTextNode('load headtracking module '));
  $div->appendChild($doc->createElement('br'));
  // apply headtracking:
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtrackingrot');
  $el->setAttribute('id','headtrackingrot');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtrackingrot'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtrackingrot');
  $el->appendChild($doc->createTextNode('apply rotation to receiver'));
  $div->appendChild($doc->createElement('br'));
  //
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','headtrackingrotsrc');
  $el->setAttribute('id','headtrackingrotsrc');
  $el->setAttribute('type','checkbox');
  if( $devprop['headtrackingrotsrc'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','headtrackingrotsrc');
  $el->appendChild($doc->createTextNode('apply rotation to source'));
  $div->appendChild($doc->createElement('br'));
  // tau auto-reference
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('auto-referencing time constant in seconds: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('value',round($devprop['headtrackingtauref'],1));
  $el->setAttribute('id','headtrackingtauref');
  $el->setAttribute('name','headtrackingtauref');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','500');
  $el->setAttribute('step','0.1');
  $div->appendChild($doc->createElement('br'));
  //
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('data logging port: '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('value',$devprop['headtrackingport']);
  $el->setAttribute('id','headtrackingport');
  $el->setAttribute('name','headtrackingport');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','65535');
  $el->setAttribute('step','1');
  // network settings:
  $form->appendChild($doc->createElement('hr'));
  $el = $form->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Network settings:'));
  $span = $el->appendChild($doc->createElement('span'));
  $span->setAttribute('class','valuechanged important');
  // jitter (send):
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','jittersend');
  $el->appendChild($doc->createTextNode('sender jitter (affects buffer length of others): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','jittersend');
  $el->setAttribute('name','jittersend');
  $el->setAttribute('type','number');
  $el->setAttribute('min','1');
  $el->setAttribute('max','250');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['jittersend']);
  $form->appendChild($doc->createElement('br'));
  // jitter (receive):
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','jitterreceive');
  $el->appendChild($doc->createTextNode('receiver jitter (affects your own buffer length): '));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','jitterreceive');
  $el->setAttribute('name','jitterreceive');
  $el->setAttribute('type','number');
  $el->setAttribute('min','1');
  $el->setAttribute('max','250');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['jitterreceive']);
  $form->appendChild($doc->createElement('br'));
  // peer-to-peer:
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','peer2peer');
  $el->setAttribute('id','peer2peer');
  $el->setAttribute('type','checkbox');
  if( $devprop['peer2peer'] )
    $el->setAttribute('checked','');
  $el = $form->appendChild($doc->createElement('label'));
  $el->setAttribute('for','peer2peer');
  $el->appendChild($doc->createTextNode('peer-to-peer mode '));
  $form->appendChild($doc->createElement('br'));
  // extra destinations:
  $div = add_expert_div($form, $doc );
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','sendlocal');
  $el->setAttribute('id','sendlocal');
  $el->setAttribute('type','checkbox');
  if( $devprop['sendlocal'] )
    $el->setAttribute('checked','');
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','sendlocal');
  $el->appendChild($doc->createTextNode('send to local IP address if in same network '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','secrec');
  $el->appendChild($doc->createTextNode('additional local receiver delay for secondary receiver (0 for no secondary receiver): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','secrec');
  $el->setAttribute('name','secrec');
  $el->setAttribute('type','number');
  $el->setAttribute('min','0');
  $el->setAttribute('max','100');
  $el->setAttribute('step','1');
  $el->setAttribute('value',$devprop['secrec']);
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->setAttribute('for','xrecport');
  $el->appendChild($doc->createTextNode('additional UDP ports forwarded to other peers (space separated list): '));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('id','xrecport');
  $el->setAttribute('name','xrecport');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[0-9 ]*');
  $el->setAttribute('value',implode(" ",$devprop['xrecport']));
  $div->appendChild($doc->createElement('br'));
  // proxy settings
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','rest_setval(\'isproxy\',this.checked);');
  $el->setAttribute('type','checkbox');
  $el->setAttribute('id','devisproxy');
  if( $devprop['isproxy'] )
    $el->setAttribute('checked','');
  $div->appendChild($doc->createTextNode('act as proxy'));
  $div->appendChild($doc->createElement('br'));
  $el = $div->appendChild($doc->createElement('input'));
  $el->setAttribute('oninput','rest_setval(\'useproxy\',this.checked);');
  $el->setAttribute('type','checkbox');
  $el->setAttribute('id','devuseproxy');
  if( boolval($devprop['useproxy']))
    $el->setAttribute('checked','');
  $div->appendChild($doc->createTextNode(' use an audio proxy if possible'));
  $div->appendChild($doc->createElement('br'));
  // frontend:
  $div->appendChild($doc->createElement('hr'));
  $el = $div->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('If required change frontend: '));
  $el = $div->appendChild($doc->createElement('select'));
  $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
  $el->setAttribute('name','jsfrontendconfig');
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{}');
  $opt->appendChild($doc->createTextNode('-- switch frontend --'));
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{"url":"http://oldbox.orlandoviols.com/","protocol":"ov","ui":"https://box.orlandoviols.com/"}');
  $opt->appendChild($doc->createTextNode('box.orlandoviols.com'));
  $opt = $el->appendChild($doc->createElement('option'));
  $opt->setAttribute('value','{"url":"http://dev.ovbox.de/","protocol":"ov","ui":"https://ovbox.de/"}');
  $opt->appendChild($doc->createTextNode('ovbox.de'));
  $div->appendChild($doc->createElement('br'));
  $div->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode('Warning: '));;
  $div->appendChild($doc->createTextNode('By selecting a frontend you may lock your device. In that case please delete the file "ov-client.cfg" on the boot partition of the SD card.'));
  $div->appendChild($doc->createElement('br'));
  // developer version:
  $inp = $div->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?usedevversion='.$device.'\';');
  $inp->setAttribute('value','switch to development version');
  $inp->setAttribute('class','uibutton');
  $div->appendChild($doc->createElement('br'));
  $a = $div->appendChild($doc->createElement('a'));
  $a->setAttribute('href','rest.php?getrawjson=');
  $a->setAttribute('target','blank');
  $a->appendChild($doc->createTextNode('show raw device configuration in new tab'));
  // save:
  $form->appendChild($doc->createElement('hr'));
  $el = $form->appendChild($doc->createElement('button'));
  $el->setAttribute('class','uibutton');
  $el->setAttribute('id','devsettingssave');
  $el->appendChild($doc->createTextNode('Save'));;
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','setdevprop');
  $el->setAttribute('value',$device);
  $el->setAttribute('type','hidden');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?\';');
  $inp->setAttribute('value','Cancel');
  $inp->setAttribute('class','uibutton');
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('onclick','location.href=\'?devreset\';');
  $inp->setAttribute('value','Reset settings');
  $inp->setAttribute('class','uibutton');
  // firmware update:
  $devset->appendChild($doc->createElement('hr'));
  $fname = '../db/clver';
  $clver = '';
  if( file_exists( $fname ) )
    $clver = trim(file_get_contents( $fname ));
  if( !empty($devprop['version']) ){
    $el = $devset->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Device version:'));
    $devset->appendChild($doc->createTextNode($devprop['version']));
    if( version_compare($clver,$devprop['version'])==0 )
      $devset->appendChild($doc->createTextNode(' - your device is up to date.'));
  }
  if(  !empty($clver) && (substr($devprop['version'],0,9)=='ovclient-') &&
       (version_compare($clver,$devprop['version'])==1)){
    $div = $devset->appendChild($doc->createElement('div'));
    $div->setAttribute('class','devprop');
    $el = $div->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Firmware update:'));
    $div->appendChild($doc->createTextNode('Your device is running version '.$devprop['version'].', the latest version is '.$clver.'. '));
    if( (version_compare($devprop['version'],'ovclient-0.4.41')==1) ){
      $div->appendChild($doc->createTextNode('Before starting the firmware update, please connect your device with a
network cable. Once started, do not disconnect your device from the
power supply or network until the firmware update is completed. The
update may take up to 30 minutes.'));
      $div->appendChild($doc->createElement('br'));
      $bold = $div->appendChild($doc->createElement('b'));
      $bold->appendChild($doc->createTextNode('Due to a problem with the SSL certificates of github it might not be
      possible to update via this page. in that case, if you need to
      update, please re-create your SD card. If in doubt please
      contact the person who provided you with the ovbox.'));
      $div->appendChild($doc->createElement('br'));
      $div->appendChild($doc->createTextNode('In the most recent version 0.6.150 this problem is solved. Version
0.5.51 is sufficient in most cases.'));
      $div->appendChild($doc->createElement('br'));
      if($devprop['age']>=20){
        $div->appendChild($doc->createTextNode('Please start your device to update the firmware.'));
        $div->appendChild($doc->createElement('br'));
      }
      $a = $div->appendChild($doc->createElement('a'));
      $a->setAttribute('target','blank');
      $a->setAttribute('href','https://raw.githubusercontent.com/gisogrimm/ov-client/master/changelog');
      $a->appendChild($doc->createTextNode('recent changes'));
      $div->appendChild($doc->createElement('br'));
      $inp = $div->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','button');
      $inp->setAttribute('onclick','location.href=\'?firmwareupdate='.$device.'\';');
      $inp->setAttribute('value','update now');
      $inp->setAttribute('class','uibutton');
    }else{
      $div->appendChild($doc->createTextNode('To update the firmware, please follow the instructions '));
      $a = $div->appendChild($doc->createElement('a'));
      $a->setAttribute('target','blank');
      $a->setAttribute('href','https://github.com/gisogrimm/ovbox/wiki/Installation');
      $a->appendChild($doc->createTextNode('here.'));
    }
  }
  // transfer ownership:
  $devset->appendChild($doc->createElement('hr'));
  $el = $devset->appendChild($doc->createElement('div'));
  $el->setAttribute('class','devproptitle');
  $el->appendChild($doc->createTextNode('Device ownership:'));
  //
  $form = $devset->appendChild($doc->createElement('form'));
  $form->setAttribute('method','POST');
  $el = $form->appendChild($doc->createElement('label'));
  $el->appendChild($doc->createTextNode('New owner:'));
  $form->appendChild($doc->createElement('br'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('type','text');
  $el->setAttribute('name','newowner');
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','transferownership');
  $el->setAttribute('value',$device);
  $el->setAttribute('type','hidden');
  $el = $form->appendChild($doc->createElement('button'));
  $el->setAttribute('class','uibutton');
  $el->appendChild($doc->createTextNode('transfer ownership'));;
  // not my device:
  $inp = $form->appendChild($doc->createElement('input'));
  $inp->setAttribute('type','button');
  $inp->setAttribute('class','roomsettingstoggle uibutton');
  $inp->setAttribute('onclick','location.href=\'?unclaim\';');
  $inp->setAttribute('value','not my device');

  // metronome:
  if( version_compare("ovclient-0.5.42-656035c",$devprop['version'])<0 ){
    $div = $root->appendChild($doc->createElement('div'));
    $div->setAttribute('id','metronome');
    $div->setAttribute('style','display: none;');
    // use IP address for mixer if possible:
    $div->setAttribute('class','mixer');
    // device is active and we know the host name:
    $div->setAttribute('style','display: block;');
    $inpactive = $div->appendChild($doc->createElement('input'));
    $inpactive->setAttribute('type','checkbox');
    if( $devprop['metroactive'] )
      $inpactive->setAttribute('checked','');
    $inpactive->setAttribute('onchange','setmetro(\'active\',this.checked);');
    $div->appendChild($doc->createTextNode(' metronome '));
    $div->appendChild($doc->createElement('br'));
    $inpbpm = $div->appendChild($doc->createElement('input'));
    $inpbpm->setAttribute('type','number');
    $inpbpm->setAttribute('min','1');
    $inpbpm->setAttribute('step','1');
    $inpbpm->setAttribute('max','400');
    $inpbpm->setAttribute('size','5');
    $inpbpm->setAttribute('value',$devprop['metrobpm']);
    $inpbpm->setAttribute('onchange','setmetro(\'bpm\',this.value);');
    $div->appendChild($doc->createTextNode(' Beats per Minute'));
    $div->appendChild($doc->createElement('br'));
    $inpbpb = $div->appendChild($doc->createElement('input'));
    $inpbpb->setAttribute('type','number');
    $inpbpb->setAttribute('min','1');
    $inpbpb->setAttribute('step','1');
    $inpbpb->setAttribute('max','20');
    $inpbpb->setAttribute('size','5');
    $inpbpb->setAttribute('value',$devprop['metrobpb']);
    $inpbpb->setAttribute('onchange','setmetro(\'bpb\',this.value);');
    $div->appendChild($doc->createTextNode(' Beats per Bar'));
    $div->appendChild($doc->createElement('br'));
    $inplevel = $div->appendChild($doc->createElement('input'));
    $inplevel->setAttribute('type','number');
    $inplevel->setAttribute('min','0');
    $inplevel->setAttribute('step','1');
    $inplevel->setAttribute('max','90');
    $inplevel->setAttribute('size','5');
    $inplevel->setAttribute('value',$devprop['metrolevel']);
    $inplevel->setAttribute('onchange','setmetro(\'level\',this.value);');
    $div->appendChild($doc->createTextNode(' level/dB'));
    $div->appendChild($doc->createElement('br'));
    $inpdelay = $div->appendChild($doc->createElement('input'));
    $inpdelay->setAttribute('type','number');
    $inpdelay->setAttribute('min','0');
    $inpdelay->setAttribute('step','1');
    $inpdelay->setAttribute('max','100');
    $inpdelay->setAttribute('size','5');
    $inpdelay->setAttribute('value',$devprop['metrodelay']);
    $inpdelay->setAttribute('onchange','setmetro(\'delay\',this.value);');
    $div->appendChild($doc->createTextNode(' local delay/ms'));
  }
  // web mixer:
  $div = $root->appendChild($doc->createElement('div'));
  $div->setAttribute('id','webmixerlink');
  $div->setAttribute('style','display: none;');
  if( $devprop['age']<20 ){
    $shown = false;
    foreach( array($devprop['localip'],$devprop['host']) as $mixer ){
      if( !(empty($mixer)||$shown) ){
        // device is active and we know the host name:
        $div->setAttribute('style','display: block;');
        $a = $div->appendChild($doc->createElement('a'));
        $a->setAttribute('href','http://'.$mixer.':8080/');
        $a->setAttribute('target','_blank');
        $a->setAttribute('rel','noopener noreferrer');
        $a->setAttribute('class','mixer');
        $div->setAttribute('class','mixer');
        $a->appendChild($doc->createTextNode('open mixer'));
        $div->appendChild($doc->createTextNode(' (works only when your browser is in the same network as your device)'));
        $shown = true;
      }
    }
  }
  echo $doc->saveHTML() . "\n";
}

function get_htpasswd( $passwdFile, $username )
{
  $lines = file($passwdFile);
  foreach ($lines as $line)
  {
    $arr = explode(":", $line);
    $fileUsername = $arr[0];
    if ($fileUsername == $username)
    {
      $filePasswd = trim($arr[1]);
      return $filePasswd;
    }
  }
  return false;
}

function get_free_chair( $room )
{
  $devs = get_devices_in_room( $room );
  $keys = array_keys( $devs );
  $k = 0;
  while( in_array( $k, $keys ) )
    $k++;
  return $k;
}

function auth( $username, $password )
{
  if( $username == 'admin' )
    return false;
  if( $username == 'device' )
    return false;
  if( $username == 'room' )
    return false;
  $fname = '../db/ovpasswd';
  $pws = array();
  if( !file_exists( $fname ) )
    return false;
  $pws = json_decode( file_get_contents( $fname ), true );
  if( !array_key_exists( $username, $pws ) ){
    // try old format:
    $filePasswd = get_htpasswd( '../ovboxusers', $username );
    return htpw_matches($password, $filePasswd);
  }
  return password_verify( $password, $pws[$username] );
}

function add_user_to_group( $user, $group )
{
  if( !(empty($group)||empty($user)) ){
    $fname = '../db/groups';
    if( file_exists( $fname ) ){
      $groups = json_decode( file_get_contents( $fname ), true );
      if( array_key_exists( $group, $groups ) ){
        $users = $groups[$group];
        if( !in_array( $user, $users ) ){
          $users[] = $user;
          $groups[$group] = $users;
          file_put_contents( $fname, json_encode( $groups ));
        }
      }
    }
  }
}

function device_enter_room( $device, $room )
{
  $othersinroom = get_devices_in_room( $room );
  if( !in_array( $device, $othersinroom ) ){
    $dprop = get_properties( $device, 'device' );
    $dprop['room'] = $room;
    $dprop['chair'] = get_free_chair($room);
    if( empty($room) )
      $dprop['roomenter'] = 0;
    else
      $dprop['roomenter'] = time();
    // store default position and orientation, in case it is not
    // overwritten by set_dev_room_pos:
    $dprop['position'] = array('x'=>0,'y'=>0,'z'=>0);
    $dprop['orientation'] = array('z'=>0,'y'=>0,'x'=>0);
    set_properties( $device, 'device', $dprop );
    set_dev_room_pos( $room );
    modify_room_prop( $room, 'deventer', time() );
  }
}

function cleanup_registrations()
{
  foreach( list_db('registration') as $r=>$dpro ){
    $fname = '../db/'.$r.'.registration';
    if( time()-filemtime($fname) > 3600 )
      unlink($fname);
  }
  foreach( list_db('pwreset') as $r=>$dpro ){
    $fname = '../db/'.$r.'.pwreset';
    if( time()-filemtime($fname) > 3600 )
      unlink($fname);
  }
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

function request_passwd_reset( $username )
{
  cleanup_registrations();
  $users = list_users();
  if( !in_array( $username, $users ) )
    return;
  $uprop = get_properties($username,'user');
  if( empty($uprop['mail']))
    return;
  $mail = $uprop['mail'];
  $reghash = md5(random_bytes(5) . $mail );
  set_properties( $reghash, 'pwreset', array('m'=>$mail,'u'=>$username) );
  $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
  $mailmsg = "Please follow this link to reset the password of your ovbox acount ".$username.":\n\n" . $actual_link . "&pwreset=".$reghash .
           "\n\nThis link is valid for one hour.\n\nIf you did not request a password reset, please ignore this mail.\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  mail( $mail, 'ovbox password reset', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
}

function register_new_user( $mail, $username, $password, $group, &$msg )
{
  if( empty($mail) ){
    $msg = 'No mail address was provided.';
    return false;
  }
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
  if( !filter_var( $mail, FILTER_VALIDATE_EMAIL ) ){
    $msg = '"'.htmlentities($mail).'" is not a valid email address.';
    return false;
  }
  cleanup_registrations();
  $registr = list_db('registration');
  $regcnt = 0;
  foreach( $registr as $rh=>$regdb )
    if( $regdb['m'] == $mail )
      $regcnt = $regcnt + 1;
  if( $regcnt > 2 ){
    $msg = 'Too many registrations for this email address. Please try again later.';
    return false;
  }
  $users = list_users();
  if( in_array( $username, $users ) ){
    $msg = 'The user "'.$username.'" already exists. Please chose another user name.';
    return false;
  }
  $reghash = md5(random_bytes(5) . $mail );
  set_properties( $reghash, 'registration', array('m'=>$mail,'u'=>$username,'g'=>$group,'p'=>password_hash( $password, PASSWORD_DEFAULT )) );
  $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
  $mailmsg = "Please follow this link to activate your ovbox acount ".$username.":\n\n" . $actual_link . "&activate=".$reghash .
           "\n\nThis link is valid for one hour.\n\nIf you did not request an activation, please ignore this mail.\n";
  $site = get_properties('site','config');
  if( $site['moderation'] )
    $mailmsg = $mailmsg."\n"."from ".$mail."\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  if( $site['moderation'] )
    mail( $site['mail'], 'ovbox account activation request', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
  else
    mail( $mail, 'ovbox account activation', $mailmsg, array('From'=>'noreply@orlandoviols.com') );
  return true;
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

function validate_pwreset( $acthash, &$pwr )
{
  $fname = '../db/'.$acthash.'.pwreset';
  if( !file_exists( $fname ) )
    return false;
  if( time()-filemtime($fname) > 3600 ){
    unlink($fname);
    return false;
  }
  $reg = get_properties($acthash,'pwreset');
  unlink($fname);
  modify_user_prop( $reg['u'], 'validpw', false);
  $pwr = $reg['u'];
  cleanup_registrations();
  return true;
}

function activate_new_user( $acthash )
{
  $fname = '../db/'.$acthash.'.registration';
  if( !file_exists( $fname ) )
    return false;
  if( time()-filemtime($fname) > 3600 ){
    unlink($fname);
    return false;
  }
  $reg = get_properties($acthash,'registration');
  unlink($fname);
  $users = list_users();
  if( in_array( $reg['u'], $users ) )
    return false;
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $pws[$reg['u']] = $reg['p'];
  file_put_contents( $fname, json_encode( $pws ));
  if( in_array($reg['g'],list_groups()) ){
    modify_user_prop( $reg['u'], 'maingroup', $reg['g']);
    add_user_to_group($reg['u'],$reg['g']);
  }
  modify_user_prop( $reg['u'], 'mail', $reg['m']);
  modify_user_prop( $reg['u'], 'agreedterms', true);
  modify_user_prop( $reg['u'], 'agreedprivacy', true);
  modify_user_prop( $reg['u'], 'access', time() );
  cleanup_registrations();
  return true;
}

function terminate_account( $user )
{
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  if( !array_key_exists($user,$pws) ){
    $sitecfg = get_properties('site','config');
    mail( $sitecfg['mail'], 'manual ovbox account activation request', 'remove user '.$user, array('From'=>'noreply@orlandoviols.com') );
  }
  unset($pws[$user]);
  file_put_contents( $fname, json_encode( $pws ));
  foreach( list_devices() as $dev=>$dprop){
    if( $dprop['owner'] == $user )
      rm_device( $dev );
  }
  foreach( list_db('room') as $room=>$dprop){
    if( $dprop['owner'] == $user )
      modify_room_prop( $room, 'owner', '' );
  }
  $fname = '../db/' . $user . '.user';
  if( file_exists( $fname ) )
    unlink($fname);
  cleanup_groups();
}

function submit_contact( $user, $contact, $msg )
{
  $mailmsg = 'From: '.$user.' ('.$contact.")\n\n".$msg."\n";
  file_put_contents( '../lastmailsg',$mailmsg);
  $sitecfg = get_properties('site','config');
  mail( $sitecfg['mail'], 'ovbox contact from '.$user, $mailmsg, array('From'=>'noreply@orlandoviols.com') );
}

function claim_device_id( $user, $dev,  &$msg )
{
  $device = str_replace( ":", "", strtolower( $dev ) );
  $fname = '../db/'.$device.'.device';
  if( file_exists( $fname ) ){
    // the user entered a listed device ID:
    $dprop = get_properties( $device, 'device' );
    if( $dprop['age'] > 20 ){
      $msg = 'The device '.$device.' is currently not active. Please power on your device to claim it. It may take up to one minute until the device is started properly.';
      return false;
    }
    if( $dprop['onlineage'] > 300 ){
      $msg = 'The device '.$device.' is online for more than five minutes. Please claim your device within the first five minutes after powering on.';
      return false;
    }
    unlink($fname);
    modify_device_prop( $device, 'version', $dprop['version']);
    modify_device_prop( $device, 'owner', $user);
    modify_device_prop( $device, 'access', $dprop['access']);
    modify_device_prop( $device, 'onlineaccess', $dprop['onlineaccess']);
    return true;
  }else{
    $msg = 'No device with MAC address or device ID '.$dev.' found in the data base. You may use the contact form to send date and time of installation, to ask for a manual identification.';
    return false;
  }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
