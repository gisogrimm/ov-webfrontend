<?php

if( !(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') ){
  if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )!= 0){
    $actual_link = "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
    header( "Location: ".$actual_link );
    die();
  }
}

include '../php/ovbox.inc';
include '../php/user.inc';
include '../php/rest.inc';
include '../php/session.inc';

if( isset($_POST['transferownership']) ){
    $device = $_POST['transferownership'];
    flock($fp_user, LOCK_EX );
    $newowner = $_POST['newowner'];
    $users = list_users();
    if( !in_array( $newowner, $users ) ){
        print_head( $user, $style, $urlgroup );
        echo '<div class="deverror">The new owner "'.$newowner.'" is not registered as a user. The ownership of device "'.$device.'" was not transferred.</div>';
        $alink = 'https://' . $_SERVER['HTTP_HOST'];
        if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
            $alink = 'http://' . $_SERVER['HTTP_HOST'];
        echo '<p><a href="'.$alink.'/device.php">Continue</a></p>' . "\n";
        print_foot($style);
    }else{
        modify_device_prop( $device, 'owner', $newowner );
        $fname = '../'.$user.'.userdevice';
        if( file_exists( $fname ) )
          unlink($fname);
        header( "Refresh:0" );
    }
    die();
}

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">Device settings</span><div class="help">Need help? - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a> / <a target="blank" href="https://forum.digital-stage.org/">DS-Forum</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';

html_show_device( $user, $device, $devprop );

if( !empty($device) ){
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  {
    // presets:
    $div = create_section($root, $doc,'Presets');
    $presets = get_properties( $device, 'devpresets' );
    unset($presets['now']);
    if( !empty($presets) ){
      $presets = array_keys($presets);
      foreach( $presets as $preset ){
        $xclass = '';
        if( $preset == $devprop['preset'] )
          $xclass = ' presetact';
        $span = $div->appendChild($doc->createElement('span'));
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
    $span = $div->appendChild($doc->createElement('span'));
    $span->setAttribute('class','presetspan');
    $inp = $span->appendChild($doc->createElement('input'));
    $inp->setAttribute('id','savepresetname');
    //$inp->setAttribute('class','presetspan');
    $inp->setAttribute('placeholder','Save current settings as preset');
    $inp = $span->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('value','Store preset');
    $inp->setAttribute('onclick','create_preset();');
  }
  {
    // general settings
    $div = create_section($root, $doc,'General settings');
    $div->appendChild($doc->createTextNode('device label (appears in rooms and the mixer of the others): '));
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('id','label');
    $el->setAttribute('name','label');
    $el->setAttribute('type','text');
    $el->setAttribute('pattern','[a-zA-Z0-9-_]*');
    $el->setAttribute('value',$devprop['label']);
    $el->setAttribute('onchange','rest_set_devprop("label",event.target.value);');
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('type','checkbox');
    if( $devprop['showexpertsettings'] )
      $el->setAttribute('checked','');
    $div->appendChild($doc->createTextNode('show expert settings (danger zone)'));
    $el->setAttribute('onchange','rest_set_devprop("showexpertsettings",event.target.checked);set_displayclass("expert",event.target.checked);');
    $el = $div->appendChild($doc->createElement('br'));
    // reset settings
    $inp = $div->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','if( confirm("Do you really want to reset settings of this device?")) rest_setval_post_reload("devreset","");');
    $inp->setAttribute('value','Reset all device settings to default values');
    $inp->setAttribute('class','uibutton');
    $divex = add_expert_div( $div, $doc, $devprop );
    $a = $divex->appendChild($doc->createElement('a'));
    $a->setAttribute('href','rest.php?getrawjson=');
    $a->setAttribute('target','blank');
    $a->appendChild($doc->createTextNode('show raw device configuration in new tab'));

  }
  {
    // Audio interface
    $div = create_section($root, $doc,'Audio interface');
    // jack device:
    $el = $div->appendChild($doc->createElement('select'));
    $el->setAttribute('id','jackdevice');
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
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
    $el->setAttribute('id','jackplugdev');
    $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    if( $devprop['jackplugdev'] )
      $el->setAttribute('checked','');
    $el = $div->appendChild($doc->createElement('label'));
    $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
    $el->setAttribute('for','jackplugdev');
    $el->appendChild($doc->createTextNode('use plugin device layer'));
    $div->appendChild($doc->createElement('br'));
    // end of alsa device.
    $el = add_input_element( $div, $doc, $devprop, 'jackrate', 'number','Sampling rate in Hz: ',false);
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $el = $div->appendChild($doc->createElement('select'));
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
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
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $divex = add_expert_div( $div, $doc, $devprop );
    $el = add_input_element( $divex, $doc, $devprop, 'jackbuffers', 'number','Number of buffers (typically 2): ');
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $el->setAttribute('min',2);
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('class','uibutton');
    $el->setAttribute('type','button');
    $el->setAttribute('value','Apply settings');
    $el->setAttribute('id','jackvaluechanged');
    $el->setAttribute('onclick','apply_jack_settings();');
    if( version_compare("ovclient-0.5.11-8cc47fd",$devprop['version'])<0 ){
      $el = $div->appendChild($doc->createElement('input'));
      $el->setAttribute('value','restart audio system');
      $el->setAttribute('type','button');
      $el->setAttribute('onclick','location.href=\'?jackrestart='.urlencode($device).'\';');
    }
  }
  {
    // connections
    $div = create_section($root, $doc,'Connections');
    create_inputportcfg( $doc, $div, $devprop );
    $divex = add_expert_div($div, $doc, $devprop);
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('output ports (to which your headphones are connected): '));
    $divex->appendChild($doc->createElement('br'));
    $el = $divex->appendChild($doc->createElement('input'));
    $el->setAttribute('type','text');
    $el->setAttribute('value',$devprop['outputport1']);
    $el->setAttribute('onchange','rest_set_devprop("outputport1",event.target.value);');
    $el = $divex->appendChild($doc->createElement('input'));
    $el->setAttribute('oninput','dispvaluechanged("valuechanged");');
    $el->setAttribute('type','text');
    $el->setAttribute('value',$devprop['outputport2']);
    $el->setAttribute('onchange','rest_set_devprop("outputport2",event.target.value);');
    $divex->appendChild($doc->createElement('br'));
    // extra ports:
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('extra ports (json expression, e.g., {"Giso:out_1":"ardour:Giso/in"}): '));
    $divex->appendChild($doc->createElement('br'));
    $el = $divex->appendChild($doc->createElement('input'));
    $el->setAttribute('size','45');
    $el->setAttribute('type','text');
    $el->setAttribute('value',$devprop['xport']);
    $el->setAttribute('onchange','rest_set_devprop("xport",event.target.value);');
    $divex->appendChild($doc->createElement('br'));
  }
  {
    // gains
    $div = create_section($root, $doc,'Gains and acoustic rendering');
    $dsl = $div->appendChild($doc->createElement('div'));
    $el = xml_add_input_generic( 'playbackgain', 'playback gain in dB (equivalent to changing the input gain): ', $div, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    // master gain:
    $el = xml_add_input_generic( 'mastergain', 'master gain in dB (equivalent to changing the headphone gain):', $div, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    // ego monitor:
    $el = xml_add_input_generic( 'egogain', 'ego monitor gain in dB (how much of your own microphone is added to your headphone):', $div, $doc, $devprop, false );
    // switch egomonitor
    xml_add_checkbox( 'selfmonitor', 'enable self monitoring', $div, $doc, $devprop, true );
    $divex = add_expert_div($div,$doc,$devprop);
    // ego monitor delay:
    $el = xml_add_input_generic( 'selfmonitordelay', 'self monitor delay in milliseconds:', $divex, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','1000');
    $el->setAttribute('step','1');
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('for','rectype');
    $el->appendChild($doc->createTextNode('receiver type: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("rectype",event.target.value);');
    $el->setAttribute('id','rectype');
    $recdesc = array('ortf'=>'Commonly used stereo microphone technique','hrtf'=>'Binaural Head Related Transfer Function simulation','itu51'=>'ITU 5.1 rendering, channel order L,R,C,LFE,Ls,Rs','omni'=>'mono');
    foreach( $recdesc as $rectype=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$rectype);
      if( $devprop['rectype'] == $rectype )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($rectype.': '.$desc));
    }
    $divex->appendChild($doc->createElement('br'));
    // reverb:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Reverb:'));
    // reverb gain:
    $el = xml_add_input_generic( 'rvbgain','extra reverb gain in dB:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    xml_add_checkbox( 'reverb', 'render reverb', $divex, $doc, $devprop, true );
    // ism
    xml_add_checkbox( 'renderism', 'render shoebox ISM', $divex, $doc, $devprop );
    // raw mode:
    xml_add_checkbox( 'rawmode', 'raw mode - no virtual acoustics', $divex, $doc, $devprop );
    // head tracking:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Head tracking:'));
    xml_add_checkbox( 'headtracking', 'load headtracking module', $divex, $doc, $devprop );
    // apply headtracking:
    xml_add_checkbox( 'headtrackingrot', 'apply rotation to receiver', $divex, $doc, $devprop );
    xml_add_checkbox( 'headtrackingrotsrc', 'apply rotation to source', $divex, $doc, $devprop );
    //
    // tau auto-reference
    $el = xml_add_input_generic( 'headtrackingtauref','auto-referencing time constant in seconds:',$divex,$doc,$devprop);
    $el->setAttribute('value',round($devprop['headtrackingtauref'],1));
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','500');
    $el->setAttribute('step','0.1');
    //
    $el = xml_add_input_generic( 'headtrackingport','data logging port for headtracking:',$divex,$doc,$devprop);
    $el->setAttribute('value',intval($devprop['headtrackingport']));
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','65535');
    $el->setAttribute('step','1');
  }
  {
    // network settings
    $div = create_section($root, $doc,'Network settings');
    // jitter (send):
    $el = xml_add_input_generic( 'jittersend','sender jitter (affects buffer length of others):',$div,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1');
    $el->setAttribute('max','250');
    $el->setAttribute('step','1');
    // jitter (receive):
    $el = xml_add_input_generic( 'jitterreceive','receiver jitter (affects your own buffer length):',$div,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1');
    $el->setAttribute('max','250');
    $el->setAttribute('step','1');
    // peer-to-peer:
    xml_add_checkbox( 'peer2peer', 'peer-to-peer mode', $div, $doc, $devprop );
    // extra destinations:
    $divex = add_expert_div($div, $doc, $devprop );
    xml_add_checkbox( 'sendlocal', 'send to local IP address if in same network', $divex, $doc, $devprop );
    $el = xml_add_input_generic( 'secrec','additional local receiver delay for secondary receiver (0 for no secondary receiver):',$divex,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','100');
    $el->setAttribute('step','1');
    $el = xml_add_input_generic( 'xrecport','additional UDP ports forwarded to other peers (space separated list):',$divex,$doc,$devprop);
    $el->setAttribute('type','text');
    $el->setAttribute('pattern','[0-9 ]*');
    // proxy settings
    if( version_compare("ovclient-0.6.120",$devprop['version'])<0 ){
      xml_add_checkbox( 'isproxy', 'offer audio proxy service to other devices in local network', $div, $doc, $devprop );
      xml_add_checkbox( 'useproxy', 'use an audio proxy if possible', $div, $doc, $devprop );
    }
    $divex = add_expert_div($div, $doc, $devprop );
    // frontend:
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('Switch configuration website: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('name','jsfrontendconfig');
    $el->setAttribute('onchange','switch_to_frontend(event.target.value);');
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','{}');
    $opt->appendChild($doc->createTextNode('-- switch frontend --'));
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','{"url":"http://oldbox.orlandoviols.com/","protocol":"ov","ui":"https://box.orlandoviols.com/"}');
    $opt->appendChild($doc->createTextNode('box.orlandoviols.com'));
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','{"url":"http://dev.ovbox.de/","protocol":"ov","ui":"https://ovbox.de/"}');
    $opt->appendChild($doc->createTextNode('ovbox.de'));
    $divex->appendChild($doc->createElement('br'));
    $divex->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode('Warning: '));;
    $divex->appendChild($doc->createTextNode('Before switching a frontend make sure you have access to the new website. By selecting a frontend you may lock your device. In that case please delete the file "ov-client.cfg" on the boot partition of the SD card.'));
    $divex->appendChild($doc->createElement('br'));
    // developer version:
    $inp = $divex->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','rest_set_devprop("usedevversion",true);');
    $inp->setAttribute('value','switch to development version');
    $inp->setAttribute('class','uibutton');
    $divex->appendChild($doc->createElement('br'));
    $a = $divex->appendChild($doc->createElement('a'));
    $a->setAttribute('href','rest.php?getrawjson=');
    $a->setAttribute('target','blank');
    $a->appendChild($doc->createTextNode('show raw device configuration in new tab'));
  }
  {
    // Firmware
    $fname = '../db/clver';
    $clver = '';
    if( file_exists( $fname ) )
      $clver = trim(file_get_contents( $fname ));
    $div = create_section($root, $doc,'Firmware version');
    if( !empty($devprop['version']) ){
      $div->appendChild($doc->createTextNode($devprop['version']));
      if( version_compare($clver,$devprop['version'])==0 )
        $div->appendChild($doc->createTextNode(' - your device is up to date.'));
    }
    if(  !empty($clver) && (substr($devprop['version'],0,9)=='ovclient-') &&
         (version_compare($clver,$devprop['version'])==1)){
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
        $inp->setAttribute('onclick','rest_set_devprop("firmwareupdate",true);');
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
  }
  {
    // device ownership:
    $div = create_section($root, $doc,'Device ownership');
    $form = $div->appendChild($doc->createElement('form'));
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
    $inp->setAttribute('onclick','rest_setval_post_reload("unclaimdevice","");');
    $inp->setAttribute('value','not my device');
  }
  echo $doc->saveHTML() . "\n";
}

print_foot($style);

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
