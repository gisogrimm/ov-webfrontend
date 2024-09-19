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
    header( "Location: /device.php" );
  }
  die();
}

if( isset($_POST['copysettings']) ){
  $device = $_POST['copysettings'];
  $srcdev = $_POST['srcdev'];
  if( !file_exists( '../db/'.$srcdev.'.device' ) ){
    print_head( $user, $style, $urlgroup );
    echo '<div class="deverror">The device "'.$srcdev.'" is not registered in the database. No settings were changed.</div>';
    $alink = 'https://' . $_SERVER['HTTP_HOST'];
    if( substr_compare( $_SERVER['HTTP_HOST'], 'localhost', 0, 9 )== 0)
      $alink = 'http://' . $_SERVER['HTTP_HOST'];
    echo '<p><a href="'.$alink.'/device.php">Continue</a></p>' . "\n";
    print_foot($style);
  }else{
    $srcprop = get_properties( $srcdev, 'device' );
    foreach( ['owner','room','label'] as $key ){
      $srcprop[$key] = $devprop[$key];
    }
    set_properties( $device, 'device', $srcprop );
    header( "Location: /device.php" );
  }
  die();
}

if( isset($_GET['claim']) ){
  $devs = list_unclaimed_devices();
  if( in_array( $_GET['claim'], $devs ) ){
    modify_device_prop( $_GET['claim'], 'owner', $user );
    select_userdev($user, $_GET['claim']);
  }
  header( "Location: /device.php" );
  die();
}

print_head( $user, $style, $urlgroup );

echo '<div><span class="ovtitle">'.translate('Device settings').'</span><div class="help">'.translate('Need help?').' - <a target="blank" href="https://github.com/gisogrimm/ovbox/wiki">Wiki-Pages</a></div></div>';

echo '<div class="devclaim" id="devclaim" style="display:none;"></div>';

html_show_device( $user, $device, $devprop );

if( !empty($device) ){
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  {
    // presets:
    $div = create_section($root, $doc,translate('Presets'));
    $presets = get_properties( $device, 'devpresets' );
    $div_presets = $div->appendChild($doc->createElement('div'));
    unset($presets['now']);
    if( !empty($presets) ){
      $presets = array_keys($presets);
      foreach( $presets as $preset ){
        $xclass = '';
        if( $preset == $devprop['preset'] )
          $xclass = ' presetact';
        $span = $div_presets->appendChild($doc->createElement('span'));
        $span->setAttribute('class','presetspan'.$xclass);
        $inp = $span->appendChild($doc->createElement('input'));
        $inp->setAttribute('class','presetload'.$xclass);
        $inp->setAttribute('type','button');
        $inp->setAttribute('value',$preset);
        $inp->setAttribute('onclick','load_preset(this.value);');
        $inp = $span->appendChild($doc->createElement('input'));
        $inp->setAttribute('class','presetrm');
        $inp->setAttribute('type','button');
        $inp->setAttribute('value','S');
        $inp->setAttribute('name',$preset);
        $inp->setAttribute('onclick','save_preset(this.name);');
        $inp = $span->appendChild($doc->createElement('input'));
        $inp->setAttribute('class','presetrm');
        $inp->setAttribute('type','button');
        $inp->setAttribute('value','X');
        $inp->setAttribute('name',$preset);
        $inp->setAttribute('onclick','rm_preset(this.name);');
      }
    }
    $div_save = $div->appendChild($doc->createElement('div'));
    // settings presets:
    //$span = $div_save->appendChild($doc->createElement('span'));
    //$span->setAttribute('class','presetspan');
    $inp = $div_save->appendChild($doc->createElement('input'));
    $inp->setAttribute('id','savepresetname');
    //$inp->setAttribute('class','presetspan');
    $inp->setAttribute('placeholder','write your preset name here');
    $inp = $div_save->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('value','Save preset');
    $inp->setAttribute('onclick','create_preset();');
  }
  {
    // general settings
    $div = create_section($root, $doc,translate('General settings'));
    $span = $div->appendChild($doc->createElement('div'));
    $span->setAttribute('class','expert');
    xml_add_checkbox( 'showexpertsettings', translate('show expert settings (danger zone)'), $span, $doc, $devprop, false, true );
    $div->appendChild($doc->createTextNode(translate('device label (appears in rooms and the mixer of the others): ')));
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('id','label');
    $el->setAttribute('name','label');
    $el->setAttribute('type','text');
    $el->setAttribute('pattern','[a-zA-Z0-9\-_]*');
    $el->setAttribute('value',$devprop['label']);
    $el->setAttribute('onchange','rest_set_devprop("label",event.target.value);');
    $div->appendChild($doc->createElement('br'));
    //$el = $div->appendChild($doc->createElement('input'));
    //$el->setAttribute('type','checkbox');
    //if( $devprop['showexpertsettings'] )
    //  $el->setAttribute('checked','');
    //$div->appendChild($doc->createTextNode('show expert settings (danger zone)'));
    //$el->setAttribute('onchange','rest_set_devprop("showexpertsettings",event.target.checked);set_displayclass("expert",event.target.checked);');
    //$el = $div->appendChild($doc->createElement('br'));
    // reset settings
    $inp = $div->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','if( confirm("Do you really want to reset settings of this device?")) rest_setval_post_reload("devreset","");');
    $inp->setAttribute('value',translate('Reset all device settings to default values'));
    $inp->setAttribute('class','uibutton');
    $divex = add_expert_div( $div, $doc, $devprop );
    $a = $divex->appendChild($doc->createElement('a'));
    $a->setAttribute('href','rest.php?getrawjson=');
    $a->setAttribute('target','blank');
    $a->appendChild($doc->createTextNode(translate('show raw device configuration in new tab')));
    $divex->appendChild($doc->createTextNode(' (device '.$device.' '.$devprop['uname_sysname'].' '.$devprop['uname_release'].' '.$devprop['uname_machine'].')'));
    $divex->appendChild($doc->createElement('br'));
    // video URLs
    $divex->appendChild($doc->createTextNode('Video: '));
    $a = $divex->appendChild($doc->createElement('a'));
    $a->setAttribute('href','https://vdo.ninja/?view='.hash('md5',$device));
    $a->setAttribute('target','blank');
    $a->appendChild($doc->createTextNode('receive URL'));
    $divex->appendChild($doc->createTextNode(' / '));
    $a = $divex->appendChild($doc->createElement('a'));
    $a->setAttribute('href','https://vdo.ninja/?push='.hash('md5',$device).'&ad=0');
    $a->setAttribute('target','blank');
    $a->appendChild($doc->createTextNode('send URL'));
    $divex->appendChild($doc->createTextNode(' (e.g., to embed in OBS)'));
    $divex->appendChild($doc->createElement('br'));
    //
    $form = $divex->appendChild($doc->createElement('form'));
    $form->setAttribute('method','POST');
    $el = $form->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('Copy settings from device:'));
    $el = $form->appendChild($doc->createElement('input'));
    $el->setAttribute('type','text');
    $el->setAttribute('name','srcdev');
    $el = $form->appendChild($doc->createElement('input'));
    $el->setAttribute('name','copysettings');
    $el->setAttribute('value',$device);
    $el->setAttribute('type','hidden');
    $el = $form->appendChild($doc->createElement('button'));
    $el->setAttribute('class','uibutton');
    $el->appendChild($doc->createTextNode('copy settings'));;

  }
  {
    // Audio interface
    $div = create_section($root, $doc,translate('Audio interface'));
    // jack device:
    $el = $div->appendChild($doc->createElement('select'));
    $el->setAttribute('id','jackdevice');
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    //
    $alsadevs = array('highest'=>'use highest device number','manual'=>'jack is started manually','dummy'=>'use virtual device (no audio i/o)','hw:1'=>'device 1 (typically first USB device)');
    if( is_array($devprop['alsadevs']) )
      $alsadevs = array_merge( $alsadevs, $devprop['alsadevs']);
    foreach( $alsadevs as $adev=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$adev);
      $opt->appendChild($doc->createTextNode($desc . ' ('.$adev.')'));
      if( $devprop['jackdevice'] == $adev )
        $opt->setAttribute('selected','');
    }
    $div->appendChild($doc->createElement('br'));
    // end of alsa device.
    $divex = add_expert_div( $div, $doc, $devprop );
    $el = add_input_element( $divex, $doc, $devprop, 'jackrate', 'number',translate('Sampling rate in Hz: '),false);
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    // use plughw device:
    $el = $divex->appendChild($doc->createElement('input'));
    $el->setAttribute('type','checkbox');
    $el->setAttribute('id','jackplugdev');
    $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    if( $devprop['jackplugdev'] )
      $el->setAttribute('checked','');
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('title','activate to use sampling rates not supported by hardware (reduces quality)');
    $el->setAttribute('for','jackplugdev');
    $el->appendChild($doc->createTextNode(translate('use plugin device layer')));
    // sampling rate list:
    $el = $div->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode(translate('Sampling rate: ')));
    $el = $div->appendChild($doc->createElement('select'));
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $el->setAttribute('onchange','update_jack_rate( this.value );');
    $opts = array('16000','22050','24000','32000','44100','48000','96000','192000');
    foreach($opts as $o){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$o);
      if( $o == $devprop['jackrate'] )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($o.' Hz'));
    }
    $div->appendChild($doc->createElement('br'));
    $divex = add_expert_div( $div, $doc, $devprop );
    $el = add_input_element( $divex, $doc, $devprop, 'jackperiod', 'number',translate('Period size in samples: (typically 2ms, i.e. 96 for 48000 Hz Sampling rate)'));
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $el = add_input_element( $divex, $doc, $devprop, 'jackbuffers', 'number',translate('Number of buffers (typically 2): '));
    $el->setAttribute('oninput','dispvaluechanged_id("jackvaluechanged");');
    $el->setAttribute('min',2);
    $el = $div->appendChild($doc->createElement('input'));
    $el->setAttribute('class','uibutton');
    $el->setAttribute('type','button');
    $el->setAttribute('value',translate('Apply settings'));
    $el->setAttribute('id','jackvaluechanged');
    $el->setAttribute('onclick','apply_jack_settings();');
    if( version_compare("ovclient-0.5.11-8cc47fd",$devprop['version'])<0 ){
      $el = $div->appendChild($doc->createElement('input'));
      $el->setAttribute('value',translate('restart audio system'));
      $el->setAttribute('type','button');
      $el->setAttribute('onclick','rest_set_devprop("jackrestart",true);');
    }
  }
  {
    // connections
    $div = create_section($root, $doc,translate('Input connections'));
    create_inputportcfg( $doc, $div, $devprop );
    $divex = add_expert_div($div, $doc, $devprop);
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode(translate('output ports (to which your headphones are connected): ')));
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
    //$el = $divex->appendChild($doc->createElement('input'));
    $el = $divex->appendChild($doc->createElement('textarea'));
    //$el->setAttribute('size','45');
    $el->setAttribute('style','width: 98%;');
    $el->setAttribute('rows','5');
    //$el->setAttribute('value',$devprop['xport']);
    $el->appendChild($doc->createTextNode(json_encode(json_decode($devprop['xport']),JSON_PRETTY_PRINT| JSON_UNESCAPED_SLASHES)));
    $el->setAttribute('onchange','rest_set_devprop("xport",event.target.value);');
    $divex->appendChild($doc->createElement('br'));
  }
  {
    // gains
    $div = create_section($root, $doc,translate('Gains and acoustic rendering'));
    $divex = add_expert_div($div,$doc,$devprop);
    // raw mode:
    xml_add_checkbox( 'virtualacoustics', 'virtual acoustics', $divex, $doc, $devprop, false, true );
    xml_add_checkbox('usebcf2000','Use BCF2000 DAW controller', $divex, $doc, $devprop, false, true);
    xml_add_checkbox( 'nochair', 'Do not use a place in the musician\'s circle', $divex, $doc, $devprop, false, true );
    //
    if( version_compare("ovclient-0.9.6",$devprop['version'])<0 ){
      $dsl = $div->appendChild($doc->createElement('p'));
      $dsl->appendChild($doc->createTextNode(translate('Output: ')));
      xml_add_checkbox( 'receive', translate('Receive audio from other members'), $dsl, $doc, $devprop, true );
    }
    $el = xml_add_input_generic( 'playbackgain', translate('playback gain in dB (equivalent to changing the input gain): '), $div, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    // master gain:
    $el = xml_add_input_generic( 'mastergain', translate('master gain in dB (equivalent to changing the headphone gain): '), $div, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    // ego monitor:
    // switch egomonitor
    $divva = add_expert_div($div,$doc,$devprop,'virtualacoustics');
    xml_add_checkbox( 'selfmonitor', translate('enable self monitoring'), $divva, $doc, $devprop, false, true );
    $divmon = add_expert_div($divva,$doc,$devprop,'selfmonitor');
    $el = xml_add_input_generic( 'egogain', translate('ego monitor gain in dB (how much of your own microphone is added to your headphone):'), $divmon, $doc, $devprop, false );
    $el->setAttribute('type','number');
    $el->setAttribute('min','-20');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    if( version_compare("ovclient-0.9.20-751bc89",$devprop['version'])<0 ){
      $divmon->appendChild($doc->createElement('br'));
      xml_add_checkbox( 'selfmonitoronlyreverb', translate('only reverb, no direct sound in self monitor'), $divmon, $doc, $devprop );
    }
    $divex = add_expert_div($divmon,$doc,$devprop);
    // ego monitor delay:
    $el = xml_add_input_generic( 'selfmonitordelay', translate('self monitor delay in milliseconds:'), $divex, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','1000');
    $el->setAttribute('step','1');
    $divva = add_expert_div($div,$doc,$devprop,'virtualacoustics');
    $divex = add_expert_div($divva,$doc,$devprop);
    // begin emptysessionmonitor
    xml_add_checkbox('emptysessionismonitor',translate('Create a live mixer when no room is selected'), $div, $doc, $devprop, false, true);
    $divmon = add_expert_div($div, $doc, $devprop, 'emptysessionismonitor');
    $divmon->setAttribute('class',$divmon->getAttribute('class').' devprop');
    $el = xml_add_input_generic( 'snmon_rvb_sx', translate('Room length / m:'), $divmon, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','300');
    $el->setAttribute('step','0.1');
    $el = xml_add_input_generic( 'snmon_rvb_sy', translate('Room width / m:'), $divmon, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','300');
    $el->setAttribute('step','0.1');
    $el = xml_add_input_generic( 'snmon_rvb_sz', translate('Room height / m:'), $divmon, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','300');
    $el->setAttribute('step','0.1');
    $el = xml_add_input_generic( 'snmon_rvb_damp', translate('Wall damping:'), $divmon, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','1');
    $el->setAttribute('step','0.01');
    $el = xml_add_input_generic( 'snmon_rvb_abs', translate('Wall absorption:'), $divmon, $doc, $devprop );
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','1');
    $el->setAttribute('step','0.01');
    // end emptysessionmonitor
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
    if( version_compare("ovclient-0.18.15",$devprop['version'])<0 ){
      $divex = add_expert_div($divva,$doc,$devprop);
      $el = $divex->appendChild($doc->createElement('div'));
      $el->setAttribute('class','devproptitle');
      //$el->appendChild($doc->createTextNode('Echo cancellation:'));
      xml_add_checkbox( 'useloudspeaker', 'use loudspeaker for playback (activates echo cancellation)',
                        $el, $doc, $devprop, false, true );
      $divex = add_expert_div($divex,$doc,$devprop,'useloudspeaker');
      $el = xml_add_input_generic( 'echoc_maxdist', 'maximum distance for echo cancellation in meter', $divex, $doc, $devprop );
      $el->setAttribute('type','number');
      $el->setAttribute('min','0');
      $el->setAttribute('max','30');
      $el->setAttribute('step','0.1');
      $el = xml_add_input_generic( 'echoc_level', 'level in dB SPL during echo cancellation measurement', $divex, $doc, $devprop );
      $el->setAttribute('type','number');
      $el->setAttribute('min','30');
      $el->setAttribute('max','90');
      $el->setAttribute('step','1');
      $el = xml_add_input_generic( 'echoc_nrep', 'number of repetitions in echo cancellation measurement', $divex, $doc, $devprop );
      $el->setAttribute('type','number');
      $el->setAttribute('min','1');
      $el->setAttribute('max','128');
      $el->setAttribute('step','1');
      $el = xml_add_input_generic( 'echoc_filterlen', 'minimum length of filters in echo cancellation, in samples', $divex, $doc, $devprop );
      $el->setAttribute('type','number');
      $el->setAttribute('min','16');
      $el->setAttribute('max','1024');
      $el->setAttribute('step','1');
    }
    // reverb:
    $divex = add_expert_div($divva,$doc,$devprop);
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
    xml_add_checkbox( 'rendersoundscape', 'render sound scapes', $divex, $doc, $devprop );
    xml_add_checkbox( 'distancelaw', 'apply distance law for gain', $divex, $doc, $devprop );
    $el = xml_add_input_generic( 'delaycomp','delay compensation distance in m:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','20');
    $el->setAttribute('step','0.1');
    $divex->appendChild($doc->createElement('br'));
    $el = xml_add_input_generic( 'decorr','decorrelation filter length in milliseconds:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','50');
    $el->setAttribute('step','1');
    $divex->appendChild($doc->createElement('br'));
    $el = xml_add_input_generic( 'fdnforwardstages','FDN feed forward path stages:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','5');
    $el->setAttribute('step','1');
    $divex->appendChild($doc->createElement('br'));
    $el = xml_add_input_generic( 'fdnorder','FDN order:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','3');
    $el->setAttribute('max','11');
    $el->setAttribute('step','1');
    $divex->appendChild($doc->createElement('br'));
    // level metering:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Level metering:'));
    // level meter time constant:
    $el = xml_add_input_generic( 'lmetertc','level meter time constant in s:',$divex,$doc,$devprop,false);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0.1');
    $el->setAttribute('max','60');
    $el->setAttribute('step','0.1');
    $divex->appendChild($doc->createElement('br'));
    // level meter mode:
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('for','lmeterfw');
    $el->appendChild($doc->createTextNode('level meter frequency weighting: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("lmeterfw",event.target.value);');
    $el->setAttribute('id','lmeterfw');
    $recdesc = array('Z'=>'unweighted','C'=>'C weighting','A'=>'A weighting');
    foreach( $recdesc as $fweight=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$fweight);
      if( $devprop['lmeterfw'] == $fweight )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($fweight.': '.$desc));
    }
    $divex->appendChild($doc->createElement('br'));
    // jack recorder:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Audio recorder:'));
    // file format:
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('for','jackrecfileformat');
    $el->appendChild($doc->createTextNode('File format: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("jackrecfileformat",event.target.value);');
    $el->setAttribute('id','jackrecfileformat');
    $recdesc = array('WAV'=>'Microsoft WAV',
                     'AIFF'=>'SGI/Apple AIFF',
                     'W64'=>'Sound forge W64',
                     'MAT5'=>'GNU Octave mat',
                     'FLAC'=>'Free Lossless Audio Codec',
                     'CAF'=>'Apple CAF');
    foreach( $recdesc as $smpfmt=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$smpfmt);
      if( $devprop['jackrecfileformat'] == $smpfmt )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($smpfmt.': '.$desc));
    }
    $divex->appendChild($doc->createElement('br'));
    // sample format:
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('for','jackrecsampleformat');
    $el->appendChild($doc->createTextNode('Sample format: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("jackrecsampleformat",event.target.value);');
    $el->setAttribute('id','jackrecsampleformat');
    $recdesc = array('PCM_16'=>'16 Bit signed integer',
                     'PCM_24'=>'24 bit signed integer',
                     'PCM_32'=>'32 bit signed integer',
                     'FLOAT'=>'32 Bit floating point (not for FLAC)',
                     'DOUBLE'=>'64 Bit floating point (not for FLAC)');
    foreach( $recdesc as $smpfmt=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$smpfmt);
      if( $devprop['jackrecsampleformat'] == $smpfmt )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($smpfmt.': '.$desc));
    }
    $divex->appendChild($doc->createElement('br'));
    // head tracking:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    //$el->appendChild($doc->createTextNode('Head tracking:'));
    xml_add_checkbox( 'headtracking', 'Head tracking', $el, $doc, $devprop, false, true );
    // apply headtracking:
    $divex = add_expert_div($divex,$doc,$devprop,'headtracking');
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
    xml_add_checkbox( 'headtrackingautorefzonly', 'Auto-referencing affects only Z-axis', $divex, $doc, $devprop );
    //
    $el = xml_add_input_generic( 'headtrackingport','data logging port for headtracking:',$divex,$doc,$devprop);
    $el->setAttribute('value',intval($devprop['headtrackingport']));
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','65535');
    $el->setAttribute('step','1');
    xml_add_input_generic( 'headtrackingtilturl','URL for tilt data:',$divex,$doc,$devprop);
    xml_add_input_generic( 'headtrackingtiltpath','path for tilt data:',$divex,$doc,$devprop);
    xml_add_input_generic( 'headtrackingtiltmap','Value map for tilt data:',$divex,$doc,$devprop);
    xml_add_input_generic( 'headtrackingeogpath','path for EOG data:',$divex,$doc,$devprop);
    // downmix:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('Downmix:'));
    xml_add_checkbox( 'receivedownmix', 'Receive downmix instead of individual channels', $divex, $doc, $devprop );
    xml_add_checkbox( 'senddownmix', 'Send downmix instead of physical inputs', $divex, $doc, $devprop );
    // multicast zita receiver:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    xml_add_checkbox( 'showmczita', 'Show multicast sender/receiver options', $el, $doc, $devprop, false, true );
    //$el->appendChild($doc->createTextNode(' '));
    $divex = add_expert_div($divex,$doc,$devprop,'showmczita');
    $el = xml_add_input_generic( 'locmcrecaddr','Multicast address:',$divex,$doc,$devprop);
    $el = xml_add_input_generic( 'locmcrecport','Port number:',$divex,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1024');
    $el->setAttribute('max','65535');
    $el->setAttribute('step','1');
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('Network device: '));
    //$divex->appendChild($doc->createElement('br'));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("locmcrecdevice",event.target.value);');
    $el->setAttribute('id','mczitadevice');
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->appendChild($doc->createTextNode('- please select a device -'));
    foreach( $devprop['networkdevices'] as $netdev ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$netdev);
      if( $devprop['locmcrecdevice'] == $netdev )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($netdev));
    }
    $divex->appendChild($doc->createElement('br'));
    $divex->appendChild($doc->createElement('hr'));
    xml_add_checkbox( 'uselocmcrec', 'Start zita-n2j (receiver)', $divex, $doc, $devprop, false, true );
    $el = $divex->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('Channels: '));
    $divex->appendChild($doc->createElement('br'));
    $el = $divex->appendChild($doc->createElement('input'));
    $chan = '';
    foreach($devprop['locmcrecchannels'] as $ch){
      if( !empty($chan) )
        $chan = $chan.', ';
      $chan = $chan.strval($ch);
    }
    $el->setAttribute('value',$chan);
    $el->setAttribute('pattern','[0-9 ,]*');
    $el->setAttribute('onchange','rest_set_devprop("locmcrecchannels",JSON.parse("["+event.target.value+"]"));');
    $divex->appendChild($doc->createElement('br'));
    $el = xml_add_input_generic( 'locmcrecbuffer','Receiver buffer length in ms:',$divex,$doc,$devprop);
    $el->setAttribute('value',intval($devprop['locmcrecbuffer']));
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('step','1');
    xml_add_checkbox( 'locmcrecautoconnect', 'Auto-connect receiver outputs to hardware outputs', $divex, $doc, $devprop );
    $divex->appendChild($doc->createElement('hr'));
    xml_add_checkbox( 'uselocmcsend', 'Start zita-j2n (sender)', $divex, $doc, $devprop, false, true );
    $el = xml_add_input_generic( 'locmcsendchannels','Number of sender channels:',$divex,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1');
    $el->setAttribute('max','64');
    $el->setAttribute('step','1');
    // tscinclude:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('User provided TASCAR configuration:'));
    $form = $divex->appendChild($doc->createElement('div'));
    $el = $form->appendChild($doc->createElement('textarea'));
    $el->setAttribute('name','tscinclude');
    $el->setAttribute('rows','8');
    $el->setAttribute('cols','60');
    $el->setAttribute('id','tscinclude');
    $el->appendChild($doc->createTextNode($devprop['tscinclude']));
    $form->appendChild($doc->createElement('br'));
    $el = $form->appendChild($doc->createElement('button'));
    $el->appendChild($doc->createTextNode('Clear'));
    $el->setAttribute('onclick','document.getElementById("tscinclude").value="";rest_set_devprop("tscinclude","");');
    $el = $form->appendChild($doc->createElement('button'));
    $el->appendChild($doc->createTextNode('Save'));
    $el->setAttribute('onclick','rest_set_devprop("tscinclude",document.getElementById("tscinclude").value);');
    // mhaconfig:
    $divex = add_expert_div($div,$doc,$devprop);
    $el = $divex->appendChild($doc->createElement('div'));
    $el->setAttribute('class','devproptitle');
    $el->appendChild($doc->createTextNode('openMHA configuration:'));
    $form = $divex->appendChild($doc->createElement('div'));
    $el = $form->appendChild($doc->createElement('textarea'));
    $el->setAttribute('name','mhaconfig');
    $el->setAttribute('rows','8');
    $el->setAttribute('cols','60');
    $el->setAttribute('id','mhaconfig');
    $el->appendChild($doc->createTextNode($devprop['mhaconfig']));
    $form->appendChild($doc->createElement('br'));
    $el = $form->appendChild($doc->createElement('button'));
    $el->appendChild($doc->createTextNode('Clear'));
    $el->setAttribute('onclick','document.getElementById("mhaconfig").value="";rest_set_devprop("mhaconfig","");');
    $el = $form->appendChild($doc->createElement('button'));
    $el->appendChild($doc->createTextNode('Save'));
    $el->setAttribute('onclick','rest_set_devprop("mhaconfig",document.getElementById("mhaconfig").value);');
  }
  {
    // network settings
    $div = create_section($root, $doc,translate('Network settings'));
    // jitter (send):
    $el = xml_add_input_generic( 'jittersend',translate('sender jitter (affects buffer length of others):'),$div,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1');
    $el->setAttribute('max','250');
    $el->setAttribute('step','1');
    // jitter (receive):
    $el = xml_add_input_generic( 'jitterreceive',translate('receiver jitter (affects your own buffer length):'),$div,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','1');
    $el->setAttribute('max','250');
    $el->setAttribute('step','1');
    // peer-to-peer:
    xml_add_checkbox( 'peer2peer', translate('peer-to-peer mode'), $div, $doc, $devprop );
    if( version_compare("ovclient-0.18.20",$devprop['version'])<0 ){
      $divex = add_expert_div($div, $doc, $devprop );
      xml_add_checkbox( 'usetcptunnel', 'use TCP tunnel to server (not in peer-to-peer mode)', $divex, $doc, $devprop );
    }
    if( version_compare("ovclient-0.19.24",$devprop['version'])<0 ){
      $divex = add_expert_div($div, $doc, $devprop );
      xml_add_checkbox( 'nozita', 'do not use zita-njbridge for audio transmission (requires manual setup of network)', $divex, $doc, $devprop );
    }
    // wifi
    if( $devprop['isovbox'] ){
      if( version_compare("ovclient-0.6.151",$devprop['version'])<0 ){
        $divex = add_expert_div($div, $doc, $devprop );
        $el = xml_add_checkbox( 'wifi', 'use WiFi (not for installation and updates)', $divex, $doc, $devprop );
        $el->setAttribute('id','wifi');
        $el->setAttribute('onchange','update_wifi();');
        $el = xml_add_input_generic( 'wifissid','WiFi SSID:',$divex,$doc,$devprop);
        $el->setAttribute('id','wifissid');
        $el->setAttribute('name','wifissid');
        $el->setAttribute('onchange','update_wifi();');
        $el = xml_add_input_generic( 'wifipasswd','WiFi passphrase (Warning: will be stored on server):',$divex,$doc,$devprop);
        $el->setAttribute('id','wifipasswd');
        $el->setAttribute('name','wifipasswd');
        //$el->setAttribute('type','password');
        //$el->setAttribute('autocomplete','off');
        $el->setAttribute('onchange','update_wifi();');
      }
    }
    // extra destinations:
    $divex = add_expert_div($div, $doc, $devprop );
    //
    $el = $divex->appendChild($doc->createElement('label'));
    $el->setAttribute('for','zitasampleformat');
    $el->appendChild($doc->createTextNode('Network sample format: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("zitasampleformat",event.target.value);');
    $el->setAttribute('id','zitasampleformat');
    $recdesc = array('16bit'=>'(default, lowest bandwidth)',
                     '24bit'=>'',
                     'float'=>'');
    foreach( $recdesc as $smpfmt=>$desc ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$smpfmt);
      if( $devprop['zitasampleformat'] == $smpfmt )
        $opt->setAttribute('selected','');
      $opt->appendChild($doc->createTextNode($smpfmt.' '.$desc));
    }
    $divex->appendChild($doc->createElement('br'));
    //
    xml_add_checkbox( 'sendlocal', 'send to local IP address if in same network', $divex, $doc, $devprop );
    $el = xml_add_input_generic( 'secrec','additional local receiver delay for secondary receiver (0 for no secondary receiver):',$divex,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','100');
    $el->setAttribute('step','1');
    $el = xml_add_input_generic( 'xrecport','additional UDP ports forwarded to other peers (space separated list):',$divex,$doc,$devprop);
    $el->setAttribute('type','text');
    $el->setAttribute('pattern','[0-9 ]*');
    xml_add_checkbox( 'expeditedforwarding', 'activate expedited forwarding PHB (RFC2598)', $divex, $doc, $devprop );
    $el = xml_add_input_generic( 'sessionportoffset','session-wide port offset to allow multiple instances on same host:',$divex,$doc,$devprop);
    $el->setAttribute('type','number');
    $el->setAttribute('min','0');
    $el->setAttribute('max','10000');
    $el->setAttribute('step','1');
    // proxy settings
    if( version_compare("ovclient-0.6.120",$devprop['version'])<0 ){
      xml_add_checkbox( 'isproxy', 'offer audio proxy service to other devices in local network', $divex, $doc, $devprop );
      xml_add_checkbox( 'useproxy', 'use an audio proxy if possible', $divex, $doc, $devprop );
    }
    xml_add_checkbox( 'hiresping', 'increase frequency of ping measurements at cost of data usage', $divex, $doc, $devprop );
    //$divex = add_expert_div($div, $doc, $devprop );
    // frontend:
    // load frontends from database:
    $frontends = json_decode( file_get_contents( '../db/frontends.db' ), true );
    // end load.
    $el = $div->appendChild($doc->createElement('hr'));
    $el = $div->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode('Switch configuration website: '));
    $el = $div->appendChild($doc->createElement('select'));
    $el->setAttribute('name','jsfrontendconfig');
    $el->setAttribute('onchange','switch_to_frontend(event.target.value);');
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','{}');
    $opt->appendChild($doc->createTextNode('-- switch frontend --'));
    foreach( $frontends as $frontend ){
      $opt = $el->appendChild($doc->createElement('option'));
      $val = json_encode( $frontend, JSON_UNESCAPED_SLASHES );
      $opt->setAttribute('value',$val);
      $opt->appendChild($doc->createTextNode($frontend['label']));
    }
    $div->appendChild($doc->createElement('br'));
    $div->appendChild($doc->createElement('b'))->appendChild($doc->createTextNode(translate('Warning: ')));;
    $div->appendChild($doc->createTextNode(translate('Before changing the front end, make sure you have registered an account on the new website. Without an account, you can lock your device by selecting a front end. In this case, please delete the file "ov-client.cfg" on the boot partition of the SD card.')));
    $div->appendChild($doc->createElement('br'));
  }
  {
    // Firmware
    $fname = '../db/clver';
    $clver = '';
    if( file_exists( $fname ) )
      $clver = trim(file_get_contents( $fname ));
    $div = create_section($root, $doc,translate('Firmware version'));
    if( !empty($devprop['version']) ){
      $span = $div->appendChild($doc->createElement('span'));
      $span->setAttribute('id','devfirmwareversion');
      $span->appendChild($doc->createTextNode($devprop['version']));
      if( version_compare($clver,$devprop['version'])<=0 )
        $div->appendChild($doc->createTextNode(translate(' - your device is up to date.')));
      if( version_compare("ovclient-0.5.50",$devprop['version'])>0 ){
        $div->appendChild($doc->createTextNode(translate(' - update is highly recommended.')));
      }
    }
    if( $devprop['isovbox'] ){
      if(  !empty($clver) && (substr($devprop['version'],0,9)=='ovclient-') &&
           (version_compare($clver,$devprop['version'])==1)){
        $el = $div->appendChild($doc->createElement('div'));
        $el->setAttribute('class','devproptitle');
        $el->appendChild($doc->createTextNode(translate('Firmware update:')));
        $div->appendChild($doc->createTextNode('Your device is running version '.$devprop['version'].', the latest version is '.$clver.'. '));
        if( (version_compare($devprop['version'],'ovclient-0.7.6')==1) ){
          $div->appendChild($doc->createTextNode(translate('Before starting the firmware update, please connect your device with a network cable. Once started, do not disconnect your device from the power supply or network until the firmware update is complete. The update may take up to 30 minutes.')));
          $div->appendChild($doc->createElement('br'));
          if( version_compare("ovclient-0.6.141",$devprop['version'])>0 ){
            $bold = $div->appendChild($doc->createElement('b'));
            $bold->appendChild($doc->createTextNode('Due to a problem with github\'s SSL certificates, it may not be
possible to update via this page. In this case please recreate your SD
card (see wiki pages). If in doubt, please contact the person you received the ovbox
from.'));
            $div->appendChild($doc->createElement('br'));
            $div->appendChild($doc->createTextNode('In the most recent version this problem is solved. Version
0.5.51 is sufficient in most cases.'));
            $div->appendChild($doc->createElement('br'));
          }
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
          $inp->setAttribute('onclick','if( confirm("Do you really want to update? This may take a long time. Please do not disconnect from power or internet until the device appears active again (typically 10-30 minutes).")){ rest_set_devprop("firmwareupdate",true); }');
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
    $divex = add_expert_div( $div, $doc, $devprop );
    //xml_add_checkbox( 'firmwareupdate', 'force firmware and system update', $divex, $doc, $devprop );
    $inp = $divex->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','rest_set_devprop("firmwareupdate",true);');
    $inp->setAttribute('value','force firmware update');
    $inp->setAttribute('class','uibutton');
    $divex->appendChild($doc->createTextNode(translate(' Force firmware and system update. Takes up to 30 minutes.')));
    $divex->appendChild($doc->createElement('br'));
    // install hearing support (www.openmha.org):
    $inp = $divex->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','if( confirm("Do you really want to update and install hearing support? This may take a long time. Please do not disconnect from power or internet until the device appears active again (typically 10-30 minutes).")){ rest_set_devprop("installopenmha",true); }');
    $inp->setAttribute('value','install hearing support');
    $inp->setAttribute('class','uibutton');
    $divex->appendChild($doc->createTextNode(' Install hearing support (www.openmha.org) and system update. Takes up to 30 minutes.'));
    $divex->appendChild($doc->createElement('br'));

    // developer version:
    $inp = $divex->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','button');
    $inp->setAttribute('onclick','rest_set_devprop("usedevversion",true);');
    $inp->setAttribute('value','switch to development version');
    $inp->setAttribute('class','uibutton');
    $divex->appendChild($doc->createTextNode(' Development version will be used until next reboot. Switching takes about 2 minutes.'));
    $divex->appendChild($doc->createElement('br'));
    // hifiberry stuff
    $divex->appendChild($doc->createTextNode('Activate HifiBerry driver: '));
    $el = $divex->appendChild($doc->createElement('select'));
    $el->setAttribute('onchange','rest_set_devprop("usehifiberry",event.target.value);');
    $opt = $el->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode('-- select model --'));
    $opt->setAttribute('selected','');
    $hifiberries = ['none','dac','dacplus','dacplushd','dacplusadc','dacplusadcpro','digi','digi-pro','amp'];
    foreach( $hifiberries as $berry ){
      $opt = $el->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$berry);
      $opt->appendChild($doc->createTextNode($berry));
    }
    $div->appendChild($doc->createElement('br'));
  }
  {
    // device ownership:
    $div = create_section($root, $doc,translate('Device ownership'));
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
