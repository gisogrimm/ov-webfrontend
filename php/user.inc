<?php
include_once "mail.inc";

/*
 * This file is included from user and admin pages
 */

function crypt_apr1_md5($plainpasswd, $salt)
{
    // APR1-MD5 encryption method (windows compatible)
    $tmp = "";
    $len = strlen($plainpasswd);
    $text = $plainpasswd . '$apr1$' . $salt;
    $bin = pack("H32", md5($plainpasswd . $salt . $plainpasswd));
    for ($i = $len; $i > 0; $i -= 16) {
        $text .= substr($bin, 0, min(16, $i));
    }
    for ($i = $len; $i > 0; $i >>= 1) {
        $text .= ($i & 1) ? chr(0) : $plainpasswd{0};
    }
    $bin = pack("H32", md5($text));
    for ($i = 0; $i < 1000; $i++) {
        $new = ($i & 1) ? $plainpasswd : $bin;
        if ($i % 3) $new .= $salt;
        if ($i % 7) $new .= $plainpasswd;
        $new .= ($i & 1) ? $bin : $plainpasswd;
        $bin = pack("H32", md5($new));
    }
    for ($i = 0; $i < 5; $i++) {
        $k = $i + 6;
        $j = $i + 12;
        if ($j == 16) $j = 5;
        $tmp = $bin[$i] . $bin[$k] . $bin[$j] . $tmp;
    }
    $tmp = chr(0) . chr(0) . $bin[11] . $tmp;
    $tmp = strtr(strrev(substr(base64_encode($tmp), 2)),
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    return "$" . "apr1" . "$" . $salt . "$" . $tmp;
}

function htpw_matches($password, $filePasswd)
{
    if (strpos($filePasswd, '$apr1') === 0) {
        // MD5
        $passParts = explode('$', $filePasswd);
        $salt = $passParts[2];
        $hashed = crypt_apr1_md5($password, $salt);
        return $hashed == $filePasswd;
    } elseif (strpos($filePasswd, '{SHA}') === 0) {
        // SHA1
        $hashed = "{SHA}" . base64_encode(sha1($password, TRUE));
        return $hashed == $filePasswd;
    } elseif (strpos($filePasswd, '$2y$') === 0) {
        // Bcrypt
        return password_verify($password, $filePasswd);
    } else {
        // Crypt
        $salt = substr($filePasswd, 0, 2);
        $hashed = crypt($password, $salt);
        return $hashed == $filePasswd;
    }
    return false;
}

function html_device_selector($user, $currentdevice, $dprop)
{
    $devs = list_unclaimed_devices();
    $doc = new DOMDocument('1.0');
    $root = $doc->appendChild($doc->createElement('div'));
    $div = $root->appendChild($doc->createElement('div'));
    $div->setAttribute('id', 'deverror');
    $div->setAttribute('class', 'deverror');
    $div->setAttribute('style', 'display: none;');
    if ((!empty($dprop['message'])) && ($dprop['age'] < 3600)) {
        $div->setAttribute('style', 'display: block;');
        $b = $div->appendChild($doc->createElement('b'));
        $b->appendChild($doc->createTextNode('Device error:'));
        $div->appendChild($doc->createElement('br'));
        $div->appendChild($doc->createTextNode($dprop['message']));
        if (strpos($dprop['message'], 'Unable to connect to the JACK server') !== false) {
            $div->appendChild($doc->createElement('br'));
            $div->appendChild($doc->createTextNode('Is your sound card connected and configured correctly?'));
        }
    }
    $div = $root->appendChild($doc->createElement('div'));
    $select = $div->appendChild($doc->createElement('select'));
    $select->setAttribute('id', 'deviceselector');
    $select->setAttribute('oninput', 'select_device(event.target.value);');
    $select->appendChild($doc->createElement('option'))->appendChild($doc->createTextNode(' -- please select a device -- '));
    $actclass = ' psvmember';
    if ($dprop['age'] < 20)
        $actclass = ' actmember';
    $select->setAttribute('class', $actclass);
    foreach (owned_devices($user) as $dev => $sdprop) {
        $opt = $select->appendChild($doc->createElement('option'));
        $opt->setAttribute('value', $dev);
        if ($currentdevice == $dev)
            $opt->setAttribute('selected', '');
        $active = '';
        if (($currentdevice != $dev) && ($sdprop['age'] < 20))
            $active = ' *active*';
        $opt->appendChild($doc->createTextNode($dev . ' (' . $sdprop['label'] . ')' . $active));
    }
    $pres = $div->appendChild($doc->createElement('span'));
    $pres->setAttribute('id', 'presetindicator');
    if (!empty($dprop['preset'])) {
        $pres->appendChild($doc->createTextNode($dprop['preset']));
        $pres->setAttribute('class', 'presetspan presetact');
    }
    $devstat = $div->appendChild($doc->createElement('div'));
    $devstat->setAttribute('class', 'devstatus');
    $devstat->setAttribute('id', 'devstatus');
    echo $doc->saveHTML();
}

function create_inputportcfg(&$doc, &$form, $devprop)
{
    // store input channel configuration:
    $el = $form->appendChild($doc->createElement('input'));
    $el->setAttribute('type', 'hidden');
    $el->setAttribute('id', 'jsinputchannels');
    $el->setAttribute('name', 'jsinputchannels');
    $el->setAttribute('value', json_encode($devprop['inputchannels']));
    // pass hw channel list to javascript:
    $el = $form->appendChild($doc->createElement('input'));
    $el->setAttribute('type', 'hidden');
    $el->setAttribute('id', 'hwinputchannels');
    $el->setAttribute('value', json_encode($devprop['hwinputchannels']));
    $div = $form->appendChild($doc->createElement('div'));
    $div->setAttribute('id', 'jsinputchannelsdiv');
    // load java scripts:
    $script = $form->appendChild($doc->createElement('script'));
    $script->setAttribute('src', '/js/inputchannels.js');
}

function add_input_element(&$parent, &$doc, &$devprop, $prop, $type, $label, $endbr = true)
{
    $el = $parent->appendChild($doc->createElement('label'));
    $el->setAttribute('for', $prop);
    $el->appendChild($doc->createTextNode($label));
    $parent->appendChild($doc->createElement('br'));
    $el = $parent->appendChild($doc->createElement('input'));
    $el->setAttribute('id', $prop);
    $el->setAttribute('name', $prop);
    $el->setAttribute('type', $type);
    $el->setAttribute('value', $devprop[$prop]);
    if ($endbr)
        $parent->appendChild($doc->createElement('br'));
    return $el;
}

function add_expert_div(&$el, &$doc, $devprop = array())
{
    $div = $el->appendChild($doc->createElement('div'));
    $div->setAttribute('class', 'expert');
    if (isset($devprop['showexpertsettings'])) {
        if ($devprop['showexpertsettings'])
            $div->setAttribute('style', 'display: block;');
        else
            $div->setAttribute('style', 'display: none;');
    } else {
        $div->setAttribute('style', 'display: none;');
    }
    return $div;
}

function xml_add_checkbox($id, $msg, $div, $doc, $devprop, $styled = false)
{
    if ($styled) {
        $lab = $div->appendChild($doc->createElement('label'));
        $lab->setAttribute('class', 'switch');
        $el = $lab->appendChild($doc->createElement('input'));
        $span = $lab->appendChild($doc->createElement('span'));
        $span->setAttribute('class', 'slider round');
        $span->appendChild($doc->createTextNode(''));
        $span->setAttribute('title', $msg);
    } else {
        $el = $div->appendChild($doc->createElement('input'));
    }
    $el->setAttribute('id', $id);
    $el->setAttribute('type', 'checkbox');
    if ($devprop[$id])
        $el->setAttribute('checked', '');
    $el->setAttribute('onchange', 'rest_set_devprop("' . $id . '",event.target.checked);');
    if (!$styled) {
        $ellab = $div->appendChild($doc->createElement('label'));
        $ellab->setAttribute('for', $id);
        $ellab->appendChild($doc->createTextNode($msg));
    }
    $div->appendChild($doc->createElement('br'));
    return $el;
}


function xml_add_input_generic($id, $msg, $div, $doc, $devprop, $bratend = true)
{
    $el = $div->appendChild($doc->createElement('label'));
    $el->appendChild($doc->createTextNode($msg));
    $div->appendChild($doc->createElement('br'));
    $el = $div->appendChild($doc->createElement('input'));
    if (is_array($devprop[$id]))
        $el->setAttribute('value', implode(" ", $devprop[$id]));
    else
        $el->setAttribute('value', $devprop[$id]);
    $el->setAttribute('onchange', 'rest_set_devprop("' . $id . '",event.target.value);');
    if ($bratend)
        $div->appendChild($doc->createElement('br'));
    return $el;
}

function set_dev_room_pos($room)
{
    $roomdev = get_devices_in_room($room, true);
    ksort($roomdev);
    $stagewidth = 360;
    $az = 0;
    $daz = $stagewidth / count($roomdev) * (pi() / 180);
    $az = $az * (pi() / 180) - 0.5 * $daz;
    $radius = 1.2;
    foreach ($roomdev as $chair => $dev) {
        $az = $az + $daz;
        $pos_x = $radius * cos(-$az);
        $pos_y = -$radius * sin(-$az);
        $dprop = get_properties($dev, 'device');
        $dprop['position'] = array('x' => $pos_x, 'y' => $pos_y, 'z' => 0);
        $dprop['orientation'] = array('z' => (180 / pi() * ($az + pi())), 'y' => 0, 'x' => 0);
        set_properties($dev, 'device', $dprop);
    }
}

function room_swap_devices($dev1, $dev2)
{
    $dprop = get_properties($dev1, 'device');
    $room = $dprop['room'];
    $members = get_devices_in_room($room);
    $chair1 = -1;
    $chair2 = -1;
    foreach ($members as $key => $dev) {
        if ($dev == $dev1)
            $chair1 = $key;
        if ($dev == $dev2)
            $chair2 = $key;
    }
    if (($chair1 >= 0) && ($chair2 >= 0)) {
        modify_device_prop($dev1, 'chair', $chair2);
        modify_device_prop($dev2, 'chair', $chair1);
        set_dev_room_pos($room);
    }
}

function lock_room($room, $device, $lock)
{
    if (in_array($device, get_devices_in_room($room))) {
        modify_room_prop($room, 'lock', $lock);
    }
}

function html_show_device($user, $device, $devprop)
{
    html_device_selector($user, $device, $devprop);
    $doc = new DOMDocument('1.0');
    $root = $doc->appendChild($doc->createElement('div'));
    // metronome:
    if ((version_compare("ovclient-0.5.42-656035c", $devprop['version']) < 0) && (!empty($device))) {
        $div = $root->appendChild($doc->createElement('div'));
        $div->setAttribute('id', 'metronome');
        $div->setAttribute('class', 'mixer');
        $div->setAttribute('style', 'display: block;');
        $inpactive = $div->appendChild($doc->createElement('input'));
        $inpactive->setAttribute('type', 'checkbox');
        if ($devprop['metroactive'])
            $inpactive->setAttribute('checked', '');
        $inpactive->setAttribute('onchange', 'setmetro(\'active\',this.checked);');
        $div->appendChild($doc->createTextNode(' metronome '));
        $div = $div->appendChild($doc->createElement('div'));
        $div->setAttribute('id', 'metrocontrols');
        if ($devprop['metroactive'])
            $div->setAttribute('style', 'display: block;');
        else
            $div->setAttribute('style', 'display: none;');
        //$div->appendChild($doc->createElement('br'));
        $inpbpm = $div->appendChild($doc->createElement('input'));
        $inpbpm->setAttribute('type', 'number');
        $inpbpm->setAttribute('min', '1');
        $inpbpm->setAttribute('step', '1');
        $inpbpm->setAttribute('max', '400');
        $inpbpm->setAttribute('size', '5');
        $inpbpm->setAttribute('value', $devprop['metrobpm']);
        $inpbpm->setAttribute('onchange', 'setmetro(\'bpm\',this.value);');
        $div->appendChild($doc->createTextNode(' Beats per Minute'));
        $div->appendChild($doc->createElement('br'));
        $inpbpb = $div->appendChild($doc->createElement('input'));
        $inpbpb->setAttribute('type', 'number');
        $inpbpb->setAttribute('min', '1');
        $inpbpb->setAttribute('step', '1');
        $inpbpb->setAttribute('max', '20');
        $inpbpb->setAttribute('size', '5');
        $inpbpb->setAttribute('value', $devprop['metrobpb']);
        $inpbpb->setAttribute('onchange', 'setmetro(\'bpb\',this.value);');
        $div->appendChild($doc->createTextNode(' Beats per Bar'));
        $div->appendChild($doc->createElement('br'));
        $inplevel = $div->appendChild($doc->createElement('input'));
        $inplevel->setAttribute('type', 'number');
        $inplevel->setAttribute('min', '0');
        $inplevel->setAttribute('step', '1');
        $inplevel->setAttribute('max', '90');
        $inplevel->setAttribute('size', '5');
        $inplevel->setAttribute('value', $devprop['metrolevel']);
        $inplevel->setAttribute('onchange', 'setmetro(\'level\',this.value);');
        $div->appendChild($doc->createTextNode(' level/dB'));
        $div->appendChild($doc->createElement('br'));
        $inpdelay = $div->appendChild($doc->createElement('input'));
        $inpdelay->setAttribute('type', 'number');
        $inpdelay->setAttribute('min', '0');
        $inpdelay->setAttribute('step', '1');
        $inpdelay->setAttribute('max', '100');
        $inpdelay->setAttribute('size', '5');
        $inpdelay->setAttribute('value', $devprop['metrodelay']);
        $inpdelay->setAttribute('onchange', 'setmetro(\'delay\',this.value);');
        $div->appendChild($doc->createTextNode(' local delay/ms'));
    }
    // web mixer:
    $div = $root->appendChild($doc->createElement('div'));
    $div->setAttribute('id', 'webmixerlink');
    $div->setAttribute('style', 'display: none;');
    if ($devprop['age'] < 20) {
        $shown = false;
        foreach (array($devprop['localip'], $devprop['host']) as $mixer) {
            if (!(empty($mixer) || $shown)) {
                // device is active and we know the host name:
                $div->setAttribute('style', 'display: block;');
                $a = $div->appendChild($doc->createElement('a'));
                $a->setAttribute('href', 'http://' . $mixer . ':8080/');
                $a->setAttribute('target', '_blank');
                $a->setAttribute('rel', 'noopener noreferrer');
                $a->setAttribute('class', 'mixer');
                $div->setAttribute('class', 'mixer');
                $a->appendChild($doc->createTextNode('open mixer'));
                $div->appendChild($doc->createTextNode(' (works only when your browser is in the same network as your device)'));
                $shown = true;
            }
        }
    }
    $inp = $root->appendChild($doc->createElement('input'));
    $inp->setAttribute('type', 'hidden');
    $inp->setAttribute('id', 'phpdeviceid');
    $inp->setAttribute('value', $device);
    echo $doc->saveHTML() . "\n";
}

function get_htpasswd($passwdFile, $username)
{
    $lines = file($passwdFile);
    foreach ($lines as $line) {
        $arr = explode(":", $line);
        $fileUsername = $arr[0];
        if ($fileUsername == $username) {
            $filePasswd = trim($arr[1]);
            return $filePasswd;
        }
    }
    return false;
}

function get_free_chair($room)
{
    $devs = get_devices_in_room($room);
    $keys = array_keys($devs);
    $k = 0;
    while (in_array($k, $keys))
        $k++;
    return $k;
}

function auth($username, $password)
{
    if ($username == 'admin')
        return false;
    if ($username == 'device')
        return false;
    if ($username == 'room')
        return false;
    $fname = '../db/ovpasswd';
    $pws = array();
    if (!file_exists($fname))
        return false;
    $pws = json_decode(file_get_contents($fname), true);
    if (!array_key_exists($username, $pws)) {
        // try old format:
        $filePasswd = get_htpasswd('../ovboxusers', $username);
        return htpw_matches($password, $filePasswd);
    }
    return password_verify($password, $pws[$username]);
}

function add_user_to_group($user, $group)
{
    if (!(empty($group) || empty($user))) {
        $fname = '../db/groups';
        if (file_exists($fname)) {
            $groups = json_decode(file_get_contents($fname), true);
            if (array_key_exists($group, $groups)) {
                $users = $groups[$group];
                if (!in_array($user, $users)) {
                    $users[] = $user;
                    $groups[$group] = $users;
                    file_put_contents($fname, json_encode($groups));
                }
            }
        }
    }
}

function device_enter_room($device, $room)
{
    $othersinroom = get_devices_in_room($room);
    if (!in_array($device, $othersinroom)) {
        $dprop = get_properties($device, 'device');
        $dprop['room'] = $room;
        $dprop['chair'] = get_free_chair($room);
        if (empty($room))
            $dprop['roomenter'] = 0;
        else
            $dprop['roomenter'] = time();
        // store default position and orientation, in case it is not
        // overwritten by set_dev_room_pos:
        $dprop['position'] = array('x' => 0, 'y' => 0, 'z' => 0);
        $dprop['orientation'] = array('z' => 0, 'y' => 0, 'x' => 0);
        set_properties($device, 'device', $dprop);
        set_dev_room_pos($room);
        modify_room_prop($room, 'deventer', time());
    }
}

function cleanup_registrations()
{
    foreach (list_db('registration') as $r => $dpro) {
        $fname = '../db/' . $r . '.registration';
        if (time() - filemtime($fname) > 3600)
            unlink($fname);
    }
    foreach (list_db('pwreset') as $r => $dpro) {
        $fname = '../db/' . $r . '.pwreset';
        if (time() - filemtime($fname) > 3600)
            unlink($fname);
    }
}

function request_passwd_reset($username)
{
    cleanup_registrations();
    $users = list_users();
    if (!in_array($username, $users))
        return;
    $uprop = get_properties($username, 'user');
    if (empty($uprop['mail']))
        return;
    $mail = $uprop['mail'];
    $reghash = md5(random_bytes(5) . $mail);
    set_properties($reghash, 'pwreset', array('m' => $mail, 'u' => $username));
    $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
    $mailmsg = "Please follow this link to reset the password of your ovbox acount " . $username . ":\n\n" . $actual_link . "&pwreset=" . $reghash .
        "\n\nThis link is valid for one hour.\n\nIf you did not request a password reset, please ignore this mail.\n";
    file_put_contents('../lastmailsg', $mailmsg);
    sendmail($mail, 'ovbox password reset', $mailmsg);
}

function register_new_user($mail, $username, $password, $group, &$msg)
{
    if (empty($mail)) {
        $msg = 'No mail address was provided.';
        return false;
    }
    if (empty($username)) {
        $msg = 'No user name was provided.';
        return false;
    }
    if (empty($password)) {
        $msg = 'No password was provided.';
        return false;
    }
    $errs = array();
    if (!checkPassword($password, $errs)) {
        foreach ($errs as $err)
            $msg = $msg . '<br/>' . $err;
        return false;
    }
    if (!filter_var($mail, FILTER_VALIDATE_EMAIL)) {
        $msg = '"' . htmlentities($mail) . '" is not a valid email address.';
        return false;
    }
    cleanup_registrations();
    $registr = list_db('registration');
    $regcnt = 0;
    foreach ($registr as $rh => $regdb)
        if ($regdb['m'] == $mail)
            $regcnt = $regcnt + 1;
    if ($regcnt > 2) {
        $msg = 'Too many registrations for this email address. Please try again later.';
        return false;
    }
    $users = list_users();
    if (in_array($username, $users)) {
        $msg = 'The user "' . $username . '" already exists. Please chose another user name.';
        return false;
    }
    $reghash = md5(random_bytes(5) . $mail);
    set_properties($reghash, 'registration', array('m' => $mail, 'u' => $username, 'g' => $group, 'p' => password_hash($password, PASSWORD_DEFAULT)));
    $actual_link = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
    $mailmsg = "Please follow this link to activate your ovbox acount " . $username . ":\n\n" . $actual_link . "&activate=" . $reghash .
        "\n\nThis link is valid for one hour.\n\nIf you did not request an activation, please ignore this mail.\n";
    $site = get_properties('site', 'config');
    if ($site['moderation'])
        $mailmsg = $mailmsg . "\n" . "from " . $mail . "\n";
    file_put_contents('../lastmailsg', $mailmsg);
    if ($site['moderation'])
        sendmail($site['mail'], 'ovbox account activation request', $mailmsg);
    else
        sendmail($mail, 'ovbox account activation', $mailmsg);
    return true;
}

function validate_pwreset($acthash, &$pwr)
{
    $fname = '../db/' . $acthash . '.pwreset';
    if (!file_exists($fname))
        return false;
    if (time() - filemtime($fname) > 3600) {
        unlink($fname);
        return false;
    }
    $reg = get_properties($acthash, 'pwreset');
    unlink($fname);
    modify_user_prop($reg['u'], 'validpw', false);
    $pwr = $reg['u'];
    cleanup_registrations();
    return true;
}

function activate_new_user($acthash)
{
    $fname = '../db/' . $acthash . '.registration';
    if (!file_exists($fname))
        return false;
    if (time() - filemtime($fname) > 3600) {
        unlink($fname);
        return false;
    }
    $reg = get_properties($acthash, 'registration');
    unlink($fname);
    $users = list_users();
    if (in_array($reg['u'], $users))
        return false;
    $fname = '../db/ovpasswd';
    $pws = array();
    if (file_exists($fname))
        $pws = json_decode(file_get_contents($fname), true);
    $pws[$reg['u']] = $reg['p'];
    file_put_contents($fname, json_encode($pws));
    if (in_array($reg['g'], list_groups())) {
        modify_user_prop($reg['u'], 'maingroup', $reg['g']);
        add_user_to_group($reg['u'], $reg['g']);
    }
    modify_user_prop($reg['u'], 'mail', $reg['m']);
    modify_user_prop($reg['u'], 'agreedterms', true);
    modify_user_prop($reg['u'], 'agreedprivacy', true);
    modify_user_prop($reg['u'], 'access', time());
    cleanup_registrations();
    return true;
}

function terminate_account($user)
{
    $fname = '../db/ovpasswd';
    $pws = array();
    if (file_exists($fname))
        $pws = json_decode(file_get_contents($fname), true);
    if (!array_key_exists($user, $pws)) {
        $sitecfg = get_properties('site', 'config');
        sendmail($sitecfg['mail'], 'manual ovbox account activation request', 'remove user ' . $user);
    }
    unset($pws[$user]);
    file_put_contents($fname, json_encode($pws));
    foreach (list_devices() as $dev => $dprop) {
        if ($dprop['owner'] == $user)
            rm_device($dev);
    }
    foreach (list_db('room') as $room => $dprop) {
        if ($dprop['owner'] == $user)
            modify_room_prop($room, 'owner', '');
    }
    $fname = '../db/' . $user . '.user';
    if (file_exists($fname))
        unlink($fname);
    cleanup_groups();
}

function claim_device_id($user, $dev, &$msg)
{
    $device = str_replace(":", "", strtolower($dev));
    $fname = '../db/' . $device . '.device';
    if (file_exists($fname)) {
        // the user entered a listed device ID:
        $dprop = get_properties($device, 'device');
        if ($dprop['age'] > 20) {
            $msg = 'The device ' . $device . ' is currently not active. Please power on your device to claim it. It may take up to one minute until the device is started properly.';
            return false;
        }
        if ($dprop['onlineage'] > 300) {
            $msg = 'The device ' . $device . ' is online for more than five minutes. Please claim your device within the first five minutes after powering on.';
            return false;
        }
        unlink($fname);
        modify_device_prop($device, 'version', $dprop['version']);
        modify_device_prop($device, 'owner', $user);
        modify_device_prop($device, 'access', $dprop['access']);
        modify_device_prop($device, 'onlineaccess', $dprop['onlineaccess']);
        return true;
    } else {
        $msg = 'No device with MAC address or device ID ' . $dev . ' found in the data base. Probably the installation of the device failed, it has no network connection or is blocked by a firewall.';
        return false;
    }
}

function html_show_my_pwreset($user, $style, $urlgroup, $userprop)
{
    if (!$userprop['validpw']) {
        print_head($user, $style, $urlgroup);
        $doc = new DOMDocument('1.0');
        $form = $doc->appendChild($doc->createElement('div'));
        $form->setAttribute('class', 'devprop');
        //$form->setAttribute('method','POST');
        //$form->setAttribute('action','?grp='.grouphash($urlgroup));
        //$lab = $form->appendChild($doc->createElement('label'));
        $form->appendChild($doc->createTextNode('New password (minimum 6 characters, 1 letter, 1 number): '));
        $form->appendChild($doc->createElement('br'));
        //echo '<input type="password" name="password"><br>';
        $inp = $form->appendChild($doc->createElement('input'));
        $inp->setAttribute('name', 'password');
        $inp->setAttribute('type', 'password');
        $inp->setAttribute('id', 'newpassword');
        $inp->setAttribute('required', '');
        $inp = $form->appendChild($doc->createElement('input'));
        $inp->setAttribute('type', 'submit');
        $inp->setAttribute('value', 'Submit');
        $inp->setAttribute('onclick', 'rest_setval_post_reload( \'updatepassword\', get_value_by_id(\'newpassword\',\'\' ));');
        echo $doc->saveHTML();
        print_foot($style);
        die();
    }
}

function print_head($user = "", $style = "", $urlgroup = "")
{
    $sitecfg = get_properties('site', 'config');
    $css = '';
    $title = $sitecfg['pagetitle'];
    if (!empty($style)) {
        $sprop = get_properties($style, 'style');
        $css = $sprop['css'];
        $title = $sprop['pagetitle'];
    }
    echo "<!DOCTYPE html>\n";
    echo '<html lang="en"><head><title>' . $title;
    if (!empty($user)) {
        echo ' - ' . $user;
    }
    echo '</title><style>';
    if (file_exists('../css/' . $sitecfg['basecss'])) {
        echo file_get_contents('../css/' . $sitecfg['basecss']);
    }
    if ((!empty($css)) && file_exists('../css/' . $css)) {
        echo file_get_contents('../css/' . $css);
    }
    echo '</style>';
    echo '<script src="/js/ovbox.js"></script>' . "\n";
    echo '<meta name="viewport" content="width=device-width, initial-scale=1"/>';
    echo '</head><body onload="starttimer();">';
    if (empty($user)) {
        echo '<div class="ovcontent">' . "\n";
        // start ad:
        #echo '<div class="ad"><b>Concert</b> 10 September, 19:00h CEST: <em>Sound out my voice!</em> Italian madrigals with diminutions <a href="https://orlandoviols.de/">orlandoviols.de</a> </div>'."\n";
        // end ad.
        echo '<h1>' . $title . '</h1>' . "\n";
    } else {
        $isadmin = in_array($user, $sitecfg['admin']);
        echo '<div class="navbar ovhead">' . "\n";
        echo '<button type="button"  onclick="toggle_ovnavh();" class="navbar-toggle">';
        echo '<span class="sr-only">Toggle navigation</span>';
        echo '<span class="icon-bar"></span>';
        echo '<span class="icon-bar"></span>';
        echo '<span class="icon-bar"></span>';
        echo '</button>';
        echo '<span class="ovnavnh">';
        echo '<h1 class="navbar" onclick="location.href=\'/\';">' . $title . '</h1>' . "\n";
        echo '<a class="ovlogout" href="/?grp=' . grouphash($urlgroup) . '&logout"><img src="logout.png" alt="Logout" class="navbaricon"/></a>';
        echo '</span>';
        echo '<span class="ovnavh" id="ovnavh">';
        echo '<a class="navbar ovnav" href="/?grp=' . grouphash($urlgroup) . '">Session</a>';
        echo '<a class="navbar ovnav" href="/device.php?grp=' . grouphash($urlgroup) . '">Device settings</a>';
        if ($isadmin) {
            echo '<a class="navbar ovnav adminarea" href="admin.php?adm=devices">Devices</a>';
            echo '<a class="navbar ovnav adminarea" href="admin.php?adm=rooms">Rooms</a>';
            echo '<a class="navbar ovnav adminarea" href="admin.php?adm=users">Users</a>';
            echo '<a class="navbar ovnav adminarea" href="admin.php?adm=groups">Groups</a>';
        }
        echo '<a class="navbar ovuser" href="/account.php?grp=' . grouphash($urlgroup) . '">' . $user . '</a>' . "\n";
        echo '</span>';
        echo '</div>' . "\n";
        echo '<div class="ovcontent">' . "\n";
        //// start ad:
        //echo '<div class="ad"><b>Concert</b> June 23rd, 20:15h CEST: <em>Royal Coronation Consort</em> with ORLANDOviols - <a href="https://www.kultur-vor-ort.com/veranstaltungen/royal-coronation-consort/" style="font-family: Helvetica;">kultur|vor|ort</a> </div>'."\n";
        //// end ad.
        if (isset($_SESSION['usermsg'])) {
            echo '<div class="deverror">' . $_SESSION['usermsg'] . '</div>';
            unset($_SESSION['usermsg']);
        }
    }
    //echo '<div class="deverror"><b>Planned service downtime</b> (server maintenance): Thursday April 22nd 8pm - Friday April 23rd 9am<br/><b>Server-Wartung</b>: Von Donnerstag 22.4. 20 Uhr bis vorraussichtlich Freitag 23.4. 9 Uhr steht diese Seite nicht zur Verfügung.</div>'."\n";
}

function print_foot($style = '', $show_donate = true)
{
    $sitecfg = get_properties('site', 'config');
    $credits = $sitecfg['credits'];
    if (!empty($style)) {
        $sprop = get_properties($style, 'style');
        $credits = $sprop['credits'];
        $show_donate = $sprop['showdonate'];
    }
    if ($show_donate) {
        echo '<div class="donate">';
        echo '<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">';
        echo '<input type="hidden" name="cmd" value="_s-xclick" />';
        echo '<input type="hidden" name="hosted_button_id" value="V63WV7SAVQ6EC" />';
        echo '<input type="image" src="https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif" border="0" name="submit" title="PayPal - The safer, easier way to pay online!" alt="Donate with PayPal button" />';
        echo '<img alt="" border="0" src="https://www.paypal.com/en_DE/i/scr/pixel.gif" width="1" height="1" />';
        echo '</form>';
        echo '</div>';
    }
    echo '</div>' . "\n";// ovcontent
    echo '<div class="ovfoot">' . "\n";
    echo '<div class="ovstage">A service by <a target="blank" href="https://orlandoviols.com/">ORLANDOviols</a> in cooperation with <a target="blank" href="https://digital-stage.org/">Digital Stage</a>.</div>';
    echo '<div class="foto">' . $credits . '</div>';
    echo '<div class="foto"><a target="blank" href="terms.php">Terms of Service</a> / <a target="blank" href="privacy.php">Privacy Policy</a> / <a target="blank" href="datenschutz.php">Datenschutzerklärung</a></div>';
    echo '</div>' . "\n";// ovfoot
    echo '</body>';
    echo '</html>';
}

function create_section($parent, $doc, $title)
{
    $div = $parent->appendChild($doc->createElement('div'));
    $div->setAttribute('class', 'ovsection');
    $h = $div->appendChild($doc->createElement('h2'));
    $h->appendChild($doc->createTextNode($title));
    $h->setAttribute('class', 'ovsectiontitle');
    return $div;
}

function clean_userdb()
{
    $rmusers = array();
    $users = list_users();
    $udb = array();
    $alldev = list_devices();
    foreach ($users as $us) {
        $uprop = get_properties($us, 'user');
        $uprop['owneddev'] = owned_devices($us, $alldev);
        $uprop['numdev'] = count($uprop['owneddev']);
        $devage = 3600 * 24 * 365 * 40 * 2;
        foreach ($uprop['owneddev'] as $d => $dprop) {
            $devage = min($devage, $dprop['age']);
        }
        $uprop['devage'] = $devage;
        $udb[$us] = $uprop;
    }
    foreach ($udb as $us => $uprop) {
        if ((($uprop['numdev'] == 0) && ($uprop['age'] > 31 * 24 * 3600)) ||
            ((($uprop['numdev'] > 0) && (($uprop['devage'] > 181 * 24 * 3600) && $uprop['age'] > 181 * 24 * 3600)))) {
            $rmusers[] = $us;
        }
    }
    foreach ($rmusers as $us) {
        terminate_account($us);
    }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
