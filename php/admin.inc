<?php

function table_add_header( &$tr, $field, $sortkey, &$doc, $type )
{
  $th = $tr->appendChild($doc->createElement('th'));
  if( $field == $sortkey ){
    $th->appendChild($doc->createTextNode('['.$field.']'));
}else{
    $a = $th->appendChild($doc->createElement('a'));
    $a->setAttribute('href','?sortby='.$field.'&category='.$type);
    $a->appendChild($doc->createTextNode($field));
  }
}

function sort_comp_key( &$db, $sortkey, $sortkey2 = '' )
{
  if( $sortkey == 'version' ){
    uasort( $db, function ( $a, $b ) use ($sortkey, $sortkey2)
    {
      $rv = version_compare( $a[$sortkey], $b[$sortkey] );
      if( $rv != 0 )
        return $rv;
      if( empty( $sortkey2 ) )
        return 0;
      if( $a[$sortkey2] > $b[$sortkey2] )
        return 1;
      if( $a[$sortkey2] < $b[$sortkey2] )
        return -1;
      return 0;
    } );
  }else{
    uasort( $db, function ( $a, $b ) use ($sortkey, $sortkey2)
    {
      if( $a[$sortkey] > $b[$sortkey] )
        return 1;
      if( $a[$sortkey] < $b[$sortkey] )
        return -1;
      if( empty( $sortkey2 ) )
        return 0;
      if( $a[$sortkey2] > $b[$sortkey2] )
        return 1;
      if( $a[$sortkey2] < $b[$sortkey2] )
        return -1;
      return 0;
    } );
  }
}

function rm_room( $room )
{
  $fname = '../db/' . $room . '.room';
  unlink($fname);
}

function rm_old_unclaimed_devices()
{
  $users = list_users();
  $alldevs = list_db('device');
  foreach( $alldevs as $dev => $rprop ){
    // operate only on inactive devices:
    if( $rprop['age'] > 3600 ){
      if( empty($rprop['owner']) )
        rm_device($dev);
      else
        if( !in_array($rprop['owner'],$users) )
          rm_device($dev);
    }
  }
}

function rm_old_rooms()
{
  $allrooms = list_db('room');
  foreach( $allrooms as $room => $rprop ){
    if( ($rprop['age'] > 3600) && empty($rprop['owner']) ){
      rm_room($room);
    }
  }
}

function rm_group( $group )
{
  if( !empty($group) ){
    $fname = '../db/groups';
    $groups = array();
    if( file_exists( $fname ) )
      $groups = json_decode( file_get_contents( $fname ), true );
    if( array_key_exists( $group, $groups ) ){
      unset($groups[$group]);
      file_put_contents( $fname, json_encode( $groups ));
    }
  }
}

function remove_user_from_group( $user, $group )
{
  if( !(empty($group)||empty($user)) ){
    $fname = '../db/groups';
    if( file_exists( $fname ) ){
      $groups = json_decode( file_get_contents( $fname ), true );
      if( array_key_exists( $group, $groups ) ){
        $users = $groups[$group];
        $us = array_search( $user, $users );
        if( $us !== false ){
          unset($users[$us]);
          $groups[$group] = $users;
          file_put_contents( $fname, json_encode( $groups ));
        }
      }
    }
  }
}

function modify_group_prop( $group, $key, $value )
{
  $dprop = get_properties( $group, 'group' );
  $dprop[$key] = $value;
  set_properties( $group, 'group', $dprop );
}

function list_styles()
{
  $styles = [];
  foreach( glob( '../db/*.style') as $style){
    $styles[] = basename($style,'.style');
  }
  return $styles;
}

function html_admin_users()
{
  $fname = '../db/ovpasswd';
  $pws = array();
  if( file_exists( $fname ) )
    $pws = json_decode( file_get_contents( $fname ), true );
  $oldusers = array();
  $fname = '../ovboxusers';
  if( file_exists( $fname ) ){
    $fcontent = file_get_contents( $fname );
    $lines = explode("\n",$fcontent);
    foreach( $lines as $line ){
      $linet = explode("#",trim($line));
      $line = $linet[0];
      if( !empty($line) ){
        $linet = explode(":",trim($line));
        if( !empty($linet[0]) ){
          if( ($linet[0]!='room') && ($linet[0]!='device') && ($linet[0]!='admin'))
            $oldusers[] = $linet[0];
        }
      }
    }
  }
  $users = list_users();
  $udb = array();
  $alldev = list_devices();
  foreach($users as $us){
    $uprop = get_properties($us,'user');
    $uprop['owneddev'] = owned_devices( $us, $alldev );
    $uprop['numdev'] = count($uprop['owneddev']);
    $uprop['numgrp'] = count(list_groups( $us ));
    $uprop['newdb'] = array_key_exists($us,$pws);
    $uprop['olddb'] = in_array($us,$oldusers);
    $uprop['uname'] = $us;
    $devage = 3600*24*365*40*2;
    foreach($uprop['owneddev'] as $d=>$dprop){
      $devage = min($devage,$dprop['age']);
    }
    $uprop['devage'] = $devage;
    $udb[$us] = $uprop;
  }
  // sort db:
  $sortprop = get_properties('list','sortkey');
  $sortkey = $sortprop['user'];
  if( $sortkey=='id' )
    ksort($udb);
  else{
    sort_comp_key( $udb, $sortprop['user'], 'uname' );
  }
  // create html:
  $doc = new DOMDocument('1.0');
  $root = $doc->createElement('div');
  $doc->appendChild($root);
  $root->appendChild($doc->createElement('a'))->setAttribute('name','users');
  $el = $root->appendChild($doc->createElement('h2'));
  $el->appendChild($doc->createTextNode('users: '));
  $s = $el->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($users).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  //
  $headerfields = array('id','age','devage','numdev','numgrp','maingroup');
  foreach( $headerfields as $field )
    table_add_header($tr,$field,$sortkey,$doc,'user');
  $tab->appendChild($doc->createTextNode("\n"));
  $usgroups = list_groups();
  foreach($udb as $us=>$uprop){
    $tr = $tab->appendChild($doc->createElement('tr'));
    if( $uprop['agreedterms'] ){
      if( (($uprop['numdev']==0) && ($uprop['age']>60*24*3600)) ||
      ((($uprop['numdev']>0) && (($uprop['devage']>240*24*3600) && $uprop['age']>240*24*3600))) ){
        $tr->setAttribute('style','background-color: #ffaaaa;');
      }
    }else{
      if( (($uprop['numdev']==0) && ($uprop['age']>60*24*3600) && ($uprop['age']<3600*24*365*40)) ||
          ((($uprop['numdev']>0) && (($uprop['devage']>240*24*3600) && $uprop['age']>240*24*3600)) && ($uprop['age']<3600*24*365*40)) ){
        $tr->setAttribute('style','background-color: #ccaaaa;');
      }else{
        $tr->setAttribute('style','background-color: #aaaaaa;');
      }
    }
    $td = $tr->appendChild($doc->createElement('td'));
    if( !$uprop['newdb'] ){
      $td->setAttribute('style','background-color: #aaaaff;');
    }else{
      if( $uprop['olddb'] ){
        $td->setAttribute('style','background-color: #ff88aa;');
      }
    }
    $td->appendChild($doc->createTextNode($us));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode(numage2str($uprop['age'])));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode(numage2str($uprop['devage'])));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($uprop['numdev']));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($uprop['numgrp']));
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','moduser');
    $inp->setAttribute('value',$us);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('title','If checked, the user can see all public rooms, otherwise only the owned rooms.');
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','seesall');
    $inp->setAttribute('id','seesall');
    if( $uprop['seesall'] )
      $inp->setAttribute('checked','');
    $inp = $form->appendChild($doc->createElement('label'));
    $inp->setAttribute('for','seesall');
    $inp->appendChild($doc->createTextNode(' sees public rooms '));

    $inp = $form->appendChild($doc->createElement('select'));
    $inp->setAttribute('name','maingroup');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- no main group -- '));
    foreach($usgroups as $grp){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$grp);
      $opt->appendChild($doc->createTextNode($grp));
      if( $uprop['maingroup'] == $grp )
        $opt->setAttribute('selected','');
    }
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('Save'));
    $tab->appendChild($doc->createTextNode("\n"));
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','adminterminateaccount');
    $inp->setAttribute('value',$us);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('delete account and all device config'));
  }
  echo $doc->saveHTML() . "\n";
}

function html_admin_rooms()
{
  $showfields = array();
  $showfields[] = 'srvjit';
  $sortprop = get_properties('list','sortkey');
  $sortkey = $sortprop['room'];
  $rooms = list_db('room');
  if( $sortkey == 'id' )
    ksort($rooms);
  else{
    sort_comp_key( $rooms, $sortkey );
  }
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->appendChild($doc->createElement('a'))->setAttribute('name','rooms');
  $h = $root->appendChild($doc->createElement('h2'));
  $h->appendChild($doc->createTextNode('rooms: '));
  $s = $h->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($rooms).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  // room id:
  $headerfields = array_merge(array('id','age','deventer'),$showfields,array('owner','group','label'));
  foreach( $headerfields as $field )
    table_add_header($tr,$field,$sortkey,$doc,'room');
  $users = list_users();
  $groups = list_groups();
  $cnt_old = 0;
  foreach( $rooms as $dev=>$dprop ){
    $tr = $tab->appendChild($doc->createElement('tr'));
    if( $dprop['age'] > 3600 ){
      if( empty($dprop['owner']) ){
        $tr->setAttribute('style','background-color: #ffaaaa;');
        $cnt_old = $cnt_old+1;
      }
    }
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dev));
    $age = numage2str($dprop['age']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($age));
    $deventer = numage2str(time()-$dprop['deventer']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($deventer));
    foreach( $showfields as $field )
      $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dprop[$field]));
    // user:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setroom'.'owner='.urlencode($dev).'&owner=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- unclaimed -- '));
    if( $dprop['owner'] == '' )
      $opt->setAttribute('selected','');
    foreach($users as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['owner'] == $us )
        $opt->setAttribute('selected','');
    }
    // group:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setroom'.'group='.urlencode($dev).'&group=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- no group -- '));
    if( $dprop['group'] == '' )
      $opt->setAttribute('selected','');
    foreach($groups as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['group'] == $us )
        $opt->setAttribute('selected','');
    }
    //label:
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','setroom'.'label');
    $inp->setAttribute('value',$dev);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','text');
    $inp->setAttribute('name','label');
    $inp->setAttribute('pattern','[a-zA-Z0-9-_]*');
    $inp->setAttribute('value',$dprop['label']);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Save'));
    // delete
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','rmroom');
    $inp->setAttribute('value',$dev);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Delete'));
  }
  if( $cnt_old > 0 ){
    $form = $root->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','rmoldrooms');
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('Remove '.$cnt_old.' inactive unclaimed room(s)'));
  }
  echo $doc->saveHTML() . "\n";
}

function html_admin_groups()
{
  cleanup_groups();
  $users = list_users();
  $grps = list_groups();
  $styles = list_styles();
  $doc = new DOMDocument('1.0');
  $root = $doc->createElement('div');
  $doc->appendChild($root);
  $root->appendChild($doc->createElement('a'))->setAttribute('name','groups');
  $el = $root->appendChild($doc->createElement('h2'));
  $el->appendChild($doc->createTextNode('groups: '));
  $s = $el->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($grps).')'));
  $form = $root->appendChild($doc->createElement('form'));
  $el = $form->appendChild($doc->createElement('input'));
  $el->setAttribute('name','addgroup');
  $el->setAttribute('type','text');
  $el->setAttribute('pattern','[a-zA-Z0-9-_]*');
  $el = $form->appendChild($doc->createElement('button'));
  $el->appendChild($doc->createTextNode('add group'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('group'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('users'));
  $th = $tr->appendChild($doc->createElement('th'));
  $th->appendChild($doc->createTextNode('style'));
  $tab->appendChild($doc->createTextNode("\n"));
  foreach($grps as $group){
    $gprop = get_properties( $group, 'group' );
    $grpusers = get_group_users( $group );
    sort($grpusers);
    $tr = $tab->appendChild($doc->createElement('tr'));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->appendChild($doc->createTextNode($group));
    $td = $tr->appendChild($doc->createElement('td'));
    $td->setAttribute('style','line-height: 2.1em;');
    foreach( $grpusers as $us ){
      $sp = $td->appendChild($doc->createElement('span'));
      $sp->setAttribute('class','psvmember');
      $sp->appendChild($doc->createTextNode($us));
      $form = $sp->appendChild($doc->createElement('form'));
      $form->setAttribute('style','display: inline;');
      $inp = $form->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','hidden');
      $inp->setAttribute('name','removeuserfromgroup');
      $inp->setAttribute('value',$group);
      $inp = $form->appendChild($doc->createElement('input'));
      $inp->setAttribute('type','hidden');
      $inp->setAttribute('name','groupuser');
      $inp->setAttribute('value',$us);
      $inp = $form->appendChild($doc->createElement('button'));
      $inp->setAttribute('class','kick');
      $inp->appendChild($doc->createTextNode('X'));
      $td->appendChild($doc->createTextNode(' '));
    }
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?addusertogroup='.urlencode($group).'&newuser=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- select a user to add -- '));
    foreach($users as $us){
      if( !in_array($us,$grpusers) ){
        $opt = $inp->appendChild($doc->createElement('option'));
        $opt->setAttribute('value',$us);
        $opt->appendChild($doc->createTextNode($us));
      }
    }

    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?setgrpstyle='.urlencode($group).'&grpstyle=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- default style -- '));
    $usgroups = list_groups($us);
    foreach($styles as $style){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$style);
      $opt->appendChild($doc->createTextNode($style));
      if( $gprop['style'] == $style )
        $opt->setAttribute('selected','');
    }

    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','rmgroup');
    $inp->setAttribute('value',$group);
    $inp = $form->appendChild($doc->createElement('button'));
    $inp->appendChild($doc->createTextNode('delete group'));
    $tab->appendChild($doc->createTextNode("\n"));
    $td = $tr->appendChild($doc->createElement('td'));
    $a = $td->appendChild($doc->createElement('a'));
    $a->setAttribute('href','https://box.orlandoviols.com/?grp='.grouphash( $group ));
    $a->appendChild($doc->createTextNode(grouphash( $group )));
  }
  echo $doc->saveHTML() . "\n";
}

function html_admin_db($type, $showfields = array())
{
  $sortprop = get_properties('list','sortkey');
  $devs = list_db($type);
  $sortkey = $sortprop[$type];
  if( $sortkey=='id' )
    ksort($devs);
  else{
    sort_comp_key( $devs, $sortkey );
  }
  $doc = new DOMDocument('1.0');
  $root = $doc->appendChild($doc->createElement('div'));
  $root->appendChild($doc->createElement('a'))->setAttribute('name',$type.'s');
  $h = $root->appendChild($doc->createElement('h2'));
  $h->appendChild($doc->createTextNode($type.'s: '));
  $s = $h->appendChild($doc->createElement('span'));
  $s->setAttribute('style','font-size: small;');
  $s->appendChild($doc->createTextNode('('.count($devs).')'));
  $tab = $root->appendChild($doc->createElement('table'));
  $tr = $tab->appendChild($doc->createElement('tr'));
  $headerfields = array_merge(array('id','age','onlineage'),$showfields,array('owner','label'));
  foreach( $headerfields as $field ){
    table_add_header($tr,$field,$sortkey,$doc,$type);
  }
  $users = list_users();
  foreach( $devs as $dev=>$dprop ){
    $tr = $tab->appendChild($doc->createElement('tr'));
    $age = numage2str($dprop['age']);
    if( $dprop['age'] < 20 )
      $tr->setAttribute('style','background-color: #dcc378;');
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dev));
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($age));
    $onlineage = numage2str($dprop['onlineage']);
    $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($onlineage));
    foreach( $showfields as $field ){
      if( $field == 'roomage' )
        $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode(numage2str($dprop[$field])));
      else
        $tr->appendChild($doc->createElement('td'))->appendChild($doc->createTextNode($dprop[$field]));
    }
    // user:
    $td = $tr->appendChild($doc->createElement('td'));
    $inp = $td->appendChild($doc->createElement('select'));
    $inp->setAttribute('oninput','location.href=\'?set'.$type.'owner='.urlencode($dev).'&owner=\'+event.target.value;');
    $opt = $inp->appendChild($doc->createElement('option'));
    $opt->setAttribute('value','');
    $opt->appendChild($doc->createTextNode(' -- unclaimed -- '));
    if( $dprop['owner'] == '' )
      $opt->setAttribute('selected','');
    foreach($users as $us){
      $opt = $inp->appendChild($doc->createElement('option'));
      $opt->setAttribute('value',$us);
      $opt->appendChild($doc->createTextNode($us));
      if( $dprop['owner'] == $us )
        $opt->setAttribute('selected','');
    }
    //label:
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','hidden');
    $inp->setAttribute('name','set'.$type.'label');
    $inp->setAttribute('value',$dev);
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','text');
    $inp->setAttribute('name','label');
    $inp->setAttribute('pattern','[a-zA-Z0-9-_]*');
    $inp->setAttribute('value',$dprop['label']);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Save'));
    // delete
    $td = $tr->appendChild($doc->createElement('td'));
    $form = $td->appendChild($doc->createElement('form'));
    $inp = $form->appendChild($doc->createElement('input'));
    $inp->setAttribute('type','checkbox');
    $inp->setAttribute('name','rm'.$type);
    $inp->setAttribute('value',$dev);
    $form->appendChild($doc->createElement('button'))->appendChild($doc->createTextNode('Delete'));
  }
  echo $doc->saveHTML() . "\n";
}

function add_group( $group )
{
  if( !empty($group) ){
    $fname = '../db/groups';
    $groups = array();
    if( file_exists( $fname ) )
      $groups = json_decode( file_get_contents( $fname ), true );
    if( !array_key_exists( $group, $groups ) ){
      $groups[$group] = array();
      file_put_contents( $fname, json_encode( $groups ));
    }
  }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * mode: php
 * End:
 */

?>
