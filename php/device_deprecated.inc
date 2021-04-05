<?php

function xml_add_sound( &$source, $doc, $attrs )
{
  $sound = $doc->createElement("sound");
  $source->appendChild($sound);
  $sound->setAttribute("maxdist","50");
  $sound->setAttribute("gainmodel","1");
  foreach( $attrs as $attr=>$value ){
    $sound->setAttribute($attr,$value);
  }
  return $sound;
}

function get_tascar_cfg( $device, $devhash )
{
  $success = false;
  $waitports = '';
  $localdevprop = get_properties( $device, 'device' );
  $amsender = issender($localdevprop);
  // create TASCAR document:
  $doc = new DOMDocument();
  $session = $doc->createElement("session");
  $doc->appendChild($session);
  $session->setAttribute('duration','36000');
  $session->setAttribute('name',$device);
  $session->setAttribute('license','CC0');
  $session->setAttribute("levelmeter_tc","0.5");
  $scene = $doc->createElement('scene');
  $scene->setAttribute('name',$device);
  $scene->setAttribute('guiscale','25');
  if( ! $localdevprop['rawmode'] ){
    $session->appendChild($scene);
    $session->setAttribute('srv_port','9871');
  }else{
    $session->setAttribute('srv_port','0');
  }
  $modules = $doc->createElement("modules");
  $session->appendChild($modules);
  // add receiver:
  $rec = $doc->createElement("receiver");
  $rec->setAttribute('type',$localdevprop['rectype']);
  if( $localdevprop['rectype'] == 'ortf' ){
    $rec->setAttribute('angle','140');
    $rec->setAttribute('f6db','12000');
    $rec->setAttribute('fmin','3000');
  }
  $rec->setAttribute("name","master");
  $rec->setAttribute("delaycomp","0.05");
  $scene->appendChild($rec);
  // master connections
  if( ! $localdevprop['rawmode'] ){
    $conn = $doc->createElement("connect");
    $conn->setAttribute("src",'render.'.$device.':master_l');
    $conn->setAttribute("dest",$localdevprop['outputport1']);
    if( !empty($localdevprop['outputport1']))
      $session->appendChild($conn);
    $conn = $doc->createElement("connect");
    $conn->setAttribute("src",'render.'.$device.':master_r');
    $conn->setAttribute("dest",$localdevprop['outputport2']);
    if( !empty($localdevprop['outputport2']))
      $session->appendChild($conn);
  }
  // room specific setup:
  if ( !empty( $localdevprop['room'] ) ){
    $room = $localdevprop['room'];
    $roomprop = get_room_prop( $room );
    $mychair = -1;
    if( $roomprop['age'] < 3600 ) {
      // get all sending devives:
      $roomdev = get_devices_in_room( $room );
      foreach( $roomdev as $chair => $dev )
        if ( $dev == $device )
          $mychair = $chair;
      // get all sending devives:
      $roomdev = get_devices_in_room( $room, true );
      // sort by chair number:
      ksort($roomdev);
      // if I am sending sit in circle, otherwise use 120 degree stage
      $stagewidth = 160;
      $az = -80;
      if( $amsender ){
        $stagewidth = 360;
        $az = 0;
      }
      $daz = $stagewidth/count($roomdev)*(pi()/180);
      $az = $az*(pi()/180)-0.5*$daz;
      $radius = 1.2;
      foreach( $roomdev as $chair => $dev ){
        $az = $az+$daz;
        $pos_x = sprintf("%1.3f",$radius * cos( $az ));
        $pos_y = sprintf("%1.3f",-$radius * sin( $az ));
        // get configuration of player device:
        $devprop = get_properties( $dev, 'device' );
        $numsource = count($devprop['inputchannels']);
        if( $numsource > 0 ){
          $dz = 0;
          $dx = 0;
          // relative gain:
          $gain = -4.41;
          $source = $doc->createElement("source");
          $scene->appendChild($source);
          $inport = '';
          if ( $dev == $device ){
            $source->setAttribute("name","ego");
            $dz = -0.3;
            $dx = 0.2;
            $gain = $devprop['egogain'];
            if( !$devprop['selfmonitor'] )
              $source->setAttribute("mute","true");
          }else{
            $source->setAttribute("name",$devprop['label'].'_'.$chair);
          }
          $gain = $gain + $devprop['playbackgain'];
          $dy = 0.7 * ($numsource > 1) * 0.5;
          if( !empty($devprop['inputport']) ){
            $inport = '';
            if ( $dev == $device )
              $inport = $devprop['inputport'];
            $srcshift = $devprop['srcshift'];
            $snd = xml_add_sound( $source, $doc, array('x'=>$dx+$srcshift['x'], 'y'=>$dy+$srcshift['y'], 'z'=>$dz+$srcshift['z'], 'connect'=>$inport, 'gain'=>$gain) );
            if ( $dev == $device ){
              $plugs = $snd->appendChild($doc->createElement('plugins'));
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_up_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_down_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
            }
          }
          if( !empty($devprop['inputport2']) ){
            $inport = '';
            if ( $dev == $device )
              $inport = $devprop['inputport2'];
            $snd = xml_add_sound( $source, $doc, array('x'=>$dx, 'y'=>-$dy, 'z'=>$dz, 'connect'=>$inport, 'gain'=>$gain) );
            if ( $dev == $device ){
              $plugs = $snd->appendChild($doc->createElement('plugins'));
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_up_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
              $sndf = $plugs->appendChild($doc->createElement('sndfile'));
              $sndf->setAttribute('name','akkord_down_48.wav');
              $sndf->setAttribute('level','60');
              $sndf->setAttribute('triggered','true');
              $sndf->setAttribute('transport','false');
            }
          }
          $xmlpos = $doc->createElement("position");
          $source->appendChild($xmlpos);
          $xmlpos->appendChild($doc->createTextNode("0 ".$pos_x." ".$pos_y." 0"));
          $xmlrot = $doc->createElement("orientation");
          $source->appendChild($xmlrot);
          $xmlrot->appendChild($doc->createTextNode("0 ".(180/pi()*(-$az+pi()))." 0 0"));
          if ( $dev == $device ){
            $mychair = $chair;
            // set receiver position:
            $xmlpos = $doc->createElement("position");
            $rec->appendChild($xmlpos);
            $xmlpos->appendChild($doc->createTextNode("0 ".$pos_x." ".$pos_y." 0"));
            $xmlrot = $doc->createElement("orientation");
            $rec->appendChild($xmlrot);
            $xmlrot->appendChild($doc->createTextNode("0 ".(180/pi()*(-$az+pi()))." 0 0"));
          }
        }
      }
      if( $localdevprop['reverb'] ){
        // reverb
        $reverb = $doc->createElement("reverb");
        $reverb->setAttribute("type","simplefdn");
        $reverb->setAttribute("volumetric",$roomprop['size']);
        $reverb->setAttribute("image","false");
        $reverb->setAttribute("fdnorder","5");
        $reverb->setAttribute("dw","60");
        $reverb->setAttribute("absorption",$roomprop['rvbabs']);
        $reverb->setAttribute("damping",$roomprop['rvbdamp']);
        $reverb->setAttribute("gain",$roomprop['rvbgain']+$localdevprop['rvbgain']);
        $scene->appendChild($reverb);
      }
      // modules
      $mod = $doc->createElement("system");
      $roomurl = explode( ":", $room );
      $myport = 4464+2*$mychair;
      $p2p = '';
      if( $localdevprop['peer2peer'] )
        $p2p = ' -2';
      if( $localdevprop['secrec'] > 0 )
        $p2p = $p2p . ' -x 100';
      $mod->setAttribute("command","../udpmirror/ovboxclient --rtprio=30 -s ".$roomprop['pin']." -d ". $roomurl[0] . " -p " . $roomurl[1] . " -c " . $mychair . " -l " . $myport . $p2p);
      $mod->setAttribute("onunload","killall ovboxclient ../udpmirror/ovboxclient");
      $modules->appendChild($mod);
      // receiver:
      foreach( $roomdev as $chair => $dev ){
        $devprop = get_properties( $dev, 'device' );
        $devuser = $devprop['label'];
        $numsource = !empty($devprop['inputport']) + !empty($devprop['inputport2']);
        if ( ( $numsource > 0 ) && ($dev != $device) ){
          $iport = 4464+2*$chair;
          $buff = $localdevprop['jitterreceive']+$devprop['jittersend'];
          $chan = '1';
          if( $numsource > 1 )
            $chan = $chan . ',2';
          // zita
          $mod = $modules->appendChild($doc->createElement('system'));
          $mod->setAttribute('command','zita-n2j --chan '.$chan.' --jname ' . $devuser . '_'.$chair.' --buf ' . $buff . ' 0.0.0.0 ' . $iport);
          $mod->setAttribute('onunload','killall zita-n2j');
          if( $localdevprop['secrec'] > 0 ){
            $mod = $modules->appendChild($doc->createElement('system'));
            $port = 'n2j_'.$chair.'_sec';
            $mod->setAttribute('command','zita-n2j --chan '.$chan.' --jname '.$port.' --buf ' . ($buff+$localdevprop['secrec']) . ' 0.0.0.0 ' . ($iport+100));
            $waitports = $waitports . ' ' . $port.':out_1';
            if( $numsource > 1 )
              $waitports = $waitports . ' ' . $port.':out_2';
            $mod = $modules->appendChild($doc->createElement('route'));
            $mod->setAttribute('name',$devuser . '_'.$chair.'_sec');
            $mod->setAttribute('channels',$numsource);
            $mod->setAttribute('gain',$devprop['playbackgain']);
            $mod->setAttribute('connect',$port.':out_[12]');
          }
          // connection
          for($k=0;$k<$numsource;$k++){
            $conn = $doc->createElement('connect');
            $port = $devuser . '_'.$chair.':out_'.($k+1);
            $waitports = $waitports . ' ' . $port;
            $conn->setAttribute('src',$port);
            if( $localdevprop['rawmode'] ){
              $conn->setAttribute('dest',$localdevprop['outputport'.($k+1)]);
              if( !empty($localdevprop['outputport'.($k+1)]))
                $session->appendChild($conn);
            }else{
              $conn->setAttribute('dest','render.'.$device.':' . $devuser . '_'.$chair.'.'.$k.'.0');
              $session->appendChild($conn);
            }
          }
        }
      }
      $numsource = !empty($localdevprop['inputport']) + !empty($localdevprop['inputport2']);
      if( $numsource > 0 ){
        $mod = $doc->createElement('system');
        //<system command=\'zita-j2n --chan 1 --jname sender --16bit 127.0.0.1 ${oport}\' onunload=\'killall zita-j2n\'/>'
        $mod->setAttribute('command','zita-j2n --chan '.$numsource.' --jname sender --16bit 127.0.0.1 ' . $myport);
        $mod->setAttribute('onunload','killall zita-j2n');
        $modules->appendChild($mod);
        if( !empty($localdevprop['inputport']) ){
          $conn = $doc->createElement('connect');
          $conn->setAttribute('src',$localdevprop['inputport']);
          $port = 'sender:in_1';
          $waitports = $waitports . ' ' . $port;
          $conn->setAttribute('dest',$port);
          $session->appendChild($conn);
        }
        if( !empty($localdevprop['inputport2']) ){
          $conn = $doc->createElement('connect');
          $conn->setAttribute('src',$localdevprop['inputport2']);
          $port = 'sender:in_'.$numsource;
          $waitports = $waitports . ' ' . $port;
          $conn->setAttribute('dest',$port);
          $session->appendChild($conn);
        }
        if( $localdevprop['secrec'] > 0 ){
          $mod = $modules->appendChild($doc->createElement('route'));
          $mod->setAttribute('name','egodelayed');
          $mod->setAttribute('channels',$numsource);
          $plgs = $mod->appendChild($doc->createElement('plugins'));
          $del = $plgs->appendChild($doc->createElement('delay'));
          $del->setAttribute('delay',0.001*($localdevprop['secrec']+$localdevprop['jitterreceive']+$localdevprop['jittersend']));
          $mod->setAttribute('gain',$localdevprop['playbackgain']);
          if( !empty($localdevprop['inputport']) ){
            $conn = $doc->createElement('connect');
            $conn->setAttribute('src',$localdevprop['inputport']);
            $conn->setAttribute('dest', 'egodelayed:in.0');
            $session->appendChild($conn);
          }
          if( !empty($localdevprop['inputport2']) ){
            $conn = $doc->createElement('connect');
            $conn->setAttribute('src',$localdevprop['inputport2']);
            $conn->setAttribute('dest','egodelayed:in.'.($numsource-1));
            $session->appendChild($conn);
          }
        }
      }
      $mod = $doc->createElement('system');
      //$mod->setAttribute('command','sleep 2;sleep 2');
      $mod->setAttribute('command','../udpmirror/jackwaitforport' . $waitports . ';sleep 1');
      $modules->appendChild($mod);
      $mod = $doc->createElement('touchosc');
      if( ! $localdevprop['rawmode'] )
        $modules->appendChild($mod);
      $mod = $doc->createElement('system');
      $mod->setAttribute('command','node bridge.js');
      if( ! $localdevprop['rawmode'] )
        $modules->appendChild($mod);
      $cmd = $modules->appendChild($doc->createElement('system'));
      $cmd->setAttribute('command','../headtracker/headtracker');
      $xport = json_decode($localdevprop['xport']);
      foreach( $xport as $xpsrc=>$xpdest ){
        $conn = $session->appendChild($doc->createElement('connect'));
        $conn->setAttribute('src',$xpsrc);
        $conn->setAttribute('dest',$xpdest);
      }
      $rec->setAttribute('gain',$localdevprop['mastergain']);
      $success = true;
    }
  }
  if( !$success ){
    // add announcement source
    $source = $doc->createElement('source');
    $scene->appendChild($source);
    $source->setAttribute('name','announce');
    $egosound = xml_add_sound($source, $doc, array('x'=>4) );
    $plugs = $doc->createElement('plugins');
    $egosound->appendChild($plugs);
    $sndfile = $doc->createElement('sndfile');
    $plugs->appendChild($sndfile);
    $sndfile->setAttribute('name','announce.flac');
    $sndfile->setAttribute('level','57');
    $sndfile->setAttribute('transport','false');
    $sndfile->setAttribute('loop','0');
  }
  $tsc = $doc->saveXML();
  $hash = hash('md5',$tsc);
  if( $hash != $devhash ){
    echo $hash . "\n";
    echo $tsc;
  }
}


/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
