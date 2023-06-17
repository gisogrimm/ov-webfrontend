<?php

include '../php/ovbox.inc';
include '../php/device.inc';

$user = getenv('ovboxuser');

if (isset($_SERVER['REMOTE_USER']))
    $user = $_SERVER['REMOTE_USER'];

if( isset($_SERVER['REDIRECT_REMOTE_USER']))
    $user = $_SERVER['REDIRECT_REMOTE_USER'];

// require a valid user:
if( empty($user) )
    die();

// device update:
if ($user == 'device') {
    // get exclusive lock of device database:
    flock($fp_dev, LOCK_EX );
    $device = '';
    if( isset($_GET['dev']) ){
        $device = $_GET['dev'];
        if( !empty($device) ){

            include '../php/device_deprecated.inc';
            $devhash = '';
            if( isset($_GET['hash']) )
                $devhash = $_GET['hash'];
            $host = '';
            if( isset($_GET['host']) )
                $host = $_GET['host'];
            get_tascar_cfg( $device, $devhash );
            // touch device file:
            $dprop = get_properties($device,'device');
            if( empty($dprop['inittime']) )
                modify_device_prop($device,'inittime',date(DATE_ATOM));
            if( (time()-$dprop['access']) > 30 )
                modify_device_prop( $device, 'onlineaccess', time() );
            modify_device_prop( $device, 'access', time() );
            modify_device_prop( $device, 'host', $host );
        }
    }
    if( isset($_POST['dev']) ){
        $device = $_POST['dev'];
        if( !empty($device) ){
            $devhash = '';
            if( isset($_GET['hash']) )
                $devhash = $_GET['hash'];
            $host = '';
            if( isset($_GET['host']) )
                $host = $_GET['host'];
            get_tascar_cfg( $device, $devhash );
            // touch device file:
            $dprop = get_properties($device,'device');
            if( empty($dprop['inittime']) )
                modify_device_prop($device,'inittime',date(DATE_ATOM));
            if( (time()-$dprop['access']) > 30 )
                modify_device_prop( $device, 'onlineaccess', time() );
            modify_device_prop( $device, 'access', time() );
            modify_device_prop( $device, 'host', $host );
        }
    }
    if( isset($_GET['devinit']) ){
        $device = $_GET['devinit'];
        if( !empty($device) ){
            $dprop = get_properties($device,'device');
            echo json_encode( $dprop );
            if( empty($dprop['inittime']) )
                modify_device_prop($device,'inittime',date(DATE_ATOM));
            /* PUT data comes in on the stdin stream */
            $putdata = fopen("php://input", "r");
            $jsdev = '';
            while ($data = fread($putdata,1024))
                $jsdev = $jsdev . $data;
            fclose($putdata);
            $jsdev = json_decode($jsdev,true);
            modify_device_prop( $device, 'alsadevs', $jsdev );
        }
    }
    if( isset($_GET['ovclientmsg']) ){
        $device = $_GET['ovclientmsg'];
        if( !empty($device) ){
            $putdata = fopen("php://input", "r");
            $devmsg = '';
            while ($data = fread($putdata,1024))
                $devmsg = $devmsg . $data;
            fclose($putdata);
            modify_device_prop( $device, 'message', $devmsg );
        }
    }
    if( isset($_GET['ovclient']) ){
        $host = '';
        if( isset($_GET['host']) )
            $host = $_GET['host'];
        $device = $_GET['ovclient'];
        if( !empty($device) ){
            $devhash = '';
            if( isset($_GET['hash']) )
                $devhash = $_GET['hash'];
            get_room_session( $device, $devhash );
            $putdata = fopen("php://input", "r");
            $jsdev = '';
            while ($data = fread($putdata,1024))
                $jsdev = $jsdev . $data;
            fclose($putdata);
            $jsdev = json_decode($jsdev,true);
            $dprop = get_properties($device,'device');
            if( empty($dprop['inittime']) )
                modify_device_prop($device,'inittime',date(DATE_ATOM));
            if( (time()-$dprop['access']) > 30 )
                modify_device_prop( $device, 'onlineaccess', time() );
            modify_device_prop( $device, 'access', time() );
            modify_device_prop( $device, 'alsadevs', $jsdev );
            modify_device_prop( $device, 'host', $host );
        }
    }
    // update an ovclient:
    if( isset($_GET['ovclient2']) ){
        $host = '';
        if( isset($_GET['host']) )
            $host = $_GET['host'];
        $device = $_GET['ovclient2'];
        if( !empty($device) ){
            $devhash = '';
            if( isset($_GET['hash']) )
                $devhash = $_GET['hash'];
            get_room_session( $device, $devhash );
            $putdata = fopen("php://input", "r");
            $jsmsg = '';
            while ($data = fread($putdata,1024))
                $jsmsg = $jsmsg . $data;
            fclose($putdata);
            $jsmsg = json_decode($jsmsg,true);
            {
                // update device settings:
                $dprop = get_properties($device,'device');
                if( empty($dprop['inittime']) )
                    $dprop['inittime'] = date(DATE_ATOM);
                if( (time()-$dprop['access']) > 30 )
                    $dprop['onlineaccess'] = time();
                $dprop['access'] = time();
                $dprop['alsadevs'] = $jsmsg['alsadevs'];
                $dprop['bandwidth'] = $jsmsg['bandwidth'];
                if( isset($jsmsg['cpuload']) )
                    $dprop['cpuload'] = $jsmsg['cpuload'];
                else
                    $dprop['cpuload'] = 0;
                if( isset($jsmsg['localip']) )
                    $dprop['localip'] = $jsmsg['localip'];
                else
                    $dprop['localip'] = '';
                if( isset($jsmsg['hwinputchannels']) )
                    $dprop['hwinputchannels'] = $jsmsg['hwinputchannels'];
                else
                    $dprop['hwinputchannels'] = '';
                $dprop['host'] = $host;
                $dprop['externalip'] = get_client_ip();
                if( isset($jsmsg['isovbox']) )
                    $dprop['isovbox'] = $jsmsg['isovbox'];
                else
                    $dprop['isovbox'] = true;
                if( isset($jsmsg['version']) )
                    $dprop['version'] = $jsmsg['version'];
                if( isset($jsmsg['networkdevices']) )
                    $dprop['networkdevices'] = $jsmsg['networkdevices'];
                if( isset($jsmsg['backendperiodsize']) )
                    $dprop['backendperiodsize'] = $jsmsg['backendperiodsize'];
                if( isset($jsmsg['backendsrate']) )
                    $dprop['backendsrate'] = $jsmsg['backendsrate'];
                //if( isset($jsmsg['effectplugincfg']) && $jsmsg['effectplugincfg'] ){
                //    error_log('-----');
                //    error_log(json_encode($jsmsg['effectplugincfg']));
                //    foreach($dprop['inputchannels'] as $chn => &$ch){
                //        foreach($ch['plugins'] as $key=>&$plug){
                //            $plug = array_merge($plug,$jsmsg['effectplugincfg'][$chn][$key]);
                //            error_log($key);
                //            error_log(json_encode($plug));
                //        }
                //        error_log(json_encode($ch['plugins']));
                //    }
                //    //error_log(json_encode(array_merge($effectarr,$jsmsg['effectplugincfg'])));
                //}
                set_properties( $device, 'device', $dprop );
                if( isset($jsmsg['pingstats']) )
                    set_properties( $device.'_'.$dprop['room'], 'pingstats', $jsmsg['pingstats'] );
            }
        }
    }
    if( isset($_GET['pluginscfg']) ){
        $device = $_GET['pluginscfg'];
        if( !empty($device) ){
            $putdata = fopen("php://input", "r");
            $jsmsg = '';
            while ($data = fread($putdata,1024))
                $jsmsg = $jsmsg . $data;
            fclose($putdata);
            $jsmsg = json_decode($jsmsg,true);
            if( !is_null($jsmsg) ){
                $dprop = get_properties($device,'device');
                foreach($dprop['inputchannels'] as $chn => &$ch){
                    foreach($ch['plugins'] as $key=>&$plug){
                        $plug = array_merge($plug,$jsmsg[$chn][$key]);
                    }
                }
                set_properties( $device, 'device', $dprop );
            }
        }
    }
    if( isset($_GET['objmixcfg']) ){
        $device = $_GET['objmixcfg'];
        if( !empty($device) ){
            $putdata = fopen("php://input", "r");
            $jsmsg = '';
            while ($data = fread($putdata,1024))
                $jsmsg = $jsmsg . $data;
            fclose($putdata);
            $jsmsg = json_decode($jsmsg,true);
            if( !is_null($jsmsg) && !is_null($jsmsg['channels']) ){
                $dprop = get_properties($device,'device');
                if( count($jsmsg['channels'])==count($dprop['inputchannels']) ){
                    foreach($dprop['inputchannels'] as $chn => &$ch){
                        $ch['position']['x'] = floatval($jsmsg['channels'][$chn]['x']);
                        $ch['position']['y'] = floatval($jsmsg['channels'][$chn]['y']);
                        $ch['position']['z'] = floatval($jsmsg['channels'][$chn]['z']);
                        $ch['gain'] = floatval($jsmsg['channels'][$chn]['gain']);
                    }
                    $dprop['rvbgain'] = floatval($jsmsg['reverbgain']);
                    set_properties( $device, 'device', $dprop );
                }
            }
        }
    }
    // register a device:
    if( isset($_GET['setver']) && isset($_GET['ver'])){
        $device = $_GET['setver'];
        if( !empty($_GET['ver']) )
            modify_device_prop($device,'version', $_GET['ver']);
        echo('OK');
    }
    // unlock database:
    flock($fp_dev, LOCK_UN );
    die();
}

// room update:
if ($user == 'room') {
    $clientip = get_client_ip();
    // get exclusive database lock:
    flock($fp_dev, LOCK_EX );
    if( isset($_GET['port']) && isset($_GET['name']) && isset($_GET['pin']) ) {
        $group = '';
        if( isset($_GET['grp']) ){
            if( in_array( $_GET['grp'], list_groups()))
                $group = $_GET['grp'];
        }
        $roomver = '';
        if( isset($_GET['version']) )
            $roomver = $_GET['version'];
        // update database entry:
        if( isset($_GET['srvjit']) )
            update_room( $clientip, $_GET['port'], $_GET['name'], $_GET['pin'], $group, $roomver, $_GET['srvjit'] );
        else
            update_room( $clientip, $_GET['port'], $_GET['name'], $_GET['pin'], $group, $roomver );
        if( isset($_GET['empty']) )
            clear_room_lat( $clientip, $_GET['port'] );
    }
    if( isset($_GET['latreport']) && isset($_GET['src']) && isset($_GET['dest']) && isset($_GET['lat']) && isset($_GET['jit']) ){
        // update latency report from room service:
        update_room_lat(
            $clientip,
            $_GET['latreport'],
            $_GET['src'],
            $_GET['dest'],
            $_GET['lat'],
            $_GET['jit']);
    }
    flock($fp_dev, LOCK_UN );
    die();
}

?>
