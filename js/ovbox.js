var roomvaluechanged = false;

function toggledisplay(id,msg){
    var x=document.getElementById(id);
    if(x.style.display==="none"){
	x.style.display="block";
	event.target.value='hide '+msg;
    }else{
	x.style.display="none";
	event.target.value='show '+msg;
    }
}

function toggledisplayclass(id,msg){
    var x=document.getElementsByClassName(id);
    for(var k=0;k<x.length;k++){
	if(x[k].style.display==="none"){
	    x[k].style.display="block";
	    event.target.value='hide '+msg;
	}else{
	    x[k].style.display="none";
	    event.target.value='show '+msg;
	}
    }
}

function escapeHtml(text) {
    var map = {
	'&': '&amp;',
	'<': '&lt;',
	'>': '&gt;',
	'"': '&quot;',
	"'": '&#039;'
    };
    return text.replace(/[&<>"']/g, function(m) { return map[m]; });
}

function add_input_to_form( form, id, name, type )
{
    var inp = form.appendChild(document.createElement('input'));
    inp.setAttribute('type',type);
    inp.setAttribute('name',name);
    inp.setAttribute('id',id+':cfg:'+name);
    inp.setAttribute('oninput','dispvaluechanged("roomvalchanged");');
    return inp;
}

function create_room_div( device, room )
{
    var id = room.id;
    // create div:
    var eroom = document.createElement('div');
    eroom.setAttribute('id',id);
    // create title:
    var tit = eroom.appendChild(document.createElement('div'));
    tit.setAttribute('id',id+':title');
    var memb = eroom.appendChild(document.createElement('div'));
    memb.setAttribute('id',id+':members');
    memb.setAttribute('class','rmembers');
    var ctl = eroom.appendChild(document.createElement('div'));
    ctl.setAttribute('id',id+':ctl');
    var rp = eroom.appendChild(document.createElement('div'));
    rp.setAttribute('id',id+':cfg');
    rp.setAttribute('style','display: none;');
    rp.setAttribute('class','devprop');
    var div = rp.appendChild(document.createElement('div'));
    div.setAttribute('class','roomsettingstitle');
    div.appendChild(document.createTextNode('Room settings:'));
    var span = div.appendChild(document.createElement('span'));
    span.setAttribute('class','roomvalchanged important');
    var form = rp.appendChild(document.createElement('form'));
    form.setAttribute('method','POST');
    var inp = add_input_to_form(form,id,'setroom','hidden');
    inp.setAttribute('value',id);
    if( device.owner == room.owner ){
	form.appendChild(document.createTextNode('Name: '));
	inp = add_input_to_form(form,id,'label','text');
	inp.setAttribute('pattern','[a-zA-Z0-9-_]*');
	inp.setAttribute('value',room.label);
	form.appendChild(document.createElement('br'));
    }
    form.appendChild(document.createTextNode('Size L x W x H [m]: '));
    inp = add_input_to_form(form,id,'sx','number');
    inp.setAttribute('step','0.1');
    inp.setAttribute('min','0');
    inp.setAttribute('max','300');
    inp.setAttribute('value',room.sx);
    inp.setAttribute('style','width: 50px;');
    inp = add_input_to_form(form,id,'sy','number');
    inp.setAttribute('step','0.1');
    inp.setAttribute('min','0');
    inp.setAttribute('max','200');
    inp.setAttribute('value',room.sy);
    inp.setAttribute('style','width: 50px;');
    inp = add_input_to_form(form,id,'sz','number');
    inp.setAttribute('step','0.1');
    inp.setAttribute('min','0');
    inp.setAttribute('max','100');
    inp.setAttribute('value',room.sz);
    inp.setAttribute('style','width: 50px;');
    form.appendChild(document.createElement('br'));
    form.appendChild(document.createTextNode('Gain / dB: '));
    inp = add_input_to_form(form,id,'rvbgain','number');
    inp.setAttribute('step','0.1');
    inp.setAttribute('min','-30');
    inp.setAttribute('max','20');
    inp.setAttribute('value',room.rvbgain);
    inp.setAttribute('style','width: 50px;');
    form.appendChild(document.createTextNode(' Damping: '));
    inp = add_input_to_form(form,id,'rvbdamp','number');
    inp.setAttribute('step','0.01');
    inp.setAttribute('min','0');
    inp.setAttribute('max','1');
    inp.setAttribute('value',room.rvbdamp);
    inp.setAttribute('style','width: 50px;');
    form.appendChild(document.createTextNode(' Absorption: '));
    inp = add_input_to_form(form,id,'rvbabs','number');
    inp.setAttribute('step','0.01');
    inp.setAttribute('min','0');
    inp.setAttribute('max','1');
    inp.setAttribute('value',room.rvbabs);
    inp.setAttribute('style','width: 50px;');
    form.appendChild(document.createElement('br'));
    // ambient sound:
    form.appendChild(document.createTextNode('Ambient sound file URL: '));
    inp = add_input_to_form(form,id,'ambientsound','text');
    inp.setAttribute('title','Sound file in Ambisonics B format, FuMa normalization');
    inp.setAttribute('value',room.ambientsound);
    form.appendChild(document.createTextNode(' Level/dB: '));
    inp = add_input_to_form(form,id,'ambientlevel','number');
    inp.setAttribute('step','1');
    inp.setAttribute('min','0');
    inp.setAttribute('max','85');
    inp.setAttribute('value',room.ambientlevel);
    inp.setAttribute('style','width: 60px;');
    form.appendChild(document.createElement('br'));
    // group:
    if( device.owner == room.owner ){
	inp = form.appendChild(document.createElement('label'));
	inp.appendChild(document.createTextNode('Group: '));
	inp = form.appendChild(document.createElement('select'));
	inp.setAttribute('oninput','dispvaluechanged("roomvalchanged");');
	inp.setAttribute('name','group');
	inp.setAttribute('id',id+':cfg:group');
	var opt = inp.appendChild(document.createElement('option'));
	opt.setAttribute('value','');
	opt.appendChild(document.createTextNode(' - public - '));
	device.usergroups.forEach( function( grp ){
	    opt = inp.appendChild(document.createElement('option'));
	    opt.setAttribute('value',grp);
	    opt.appendChild(document.createTextNode(grp));
	    if( room.group == grp )
		opt.setAttribute('selected','');
	});
	// private room:
	inp = add_input_to_form(form,id,'private','checkbox');
	if( room['private'] )
	    inp.setAttribute('checked','');
	inp = form.appendChild(document.createElement('label'));
	inp.setAttribute('for',id+':cfg:private');
	inp.appendChild(document.createTextNode('Private room - visible only to me (overrides group)'));
	form.appendChild(document.createElement('br'));
	// editable by everyone:
	inp = add_input_to_form(form,id,'editable','checkbox');
	if( room.editable )
	    inp.setAttribute('checked','');
	inp = form.appendChild(document.createElement('label'));
	inp.setAttribute('for',id+':cfg:editable');
	inp.appendChild(document.createTextNode('Editable by everyone who can see the room'));
	form.appendChild(document.createElement('br'));
    }
    // save button:
    var inp = form.appendChild(document.createElement('button'));
    inp.setAttribute('class','uibutton');
    inp.appendChild(document.createTextNode('Save'));
    inp.setAttribute('id','roomsettingssave');
    inp = form.appendChild(document.createElement('input'));
    inp.setAttribute('type','button');
    inp.setAttribute('value','Cancel');
    inp.setAttribute('onclick','location.href=\'/\';');
    inp.setAttribute('class','uibutton');
    inp = form.appendChild(document.createElement('input'));
    inp.setAttribute('type','button');
    inp.setAttribute('value','reset to defaults');
    inp.setAttribute('onclick','location.href=\'?resetroom='+encodeURI(id)+'\';');
    inp.setAttribute('class','uibutton');
    return eroom;
}

function update_room( device, room, droom )
{
    room.lock = Number(room.lock);
    var eroom = document.getElementById(room.id);
    if( !eroom )
	eroom = droom.appendChild(create_room_div(device, room));
    if( room.entered )
	eroom.setAttribute('class','myroom');
    else
	eroom.setAttribute('class','room');
    // title div:
    var tit = document.getElementById(room.id+':title');
    while( tit.firstChild ) tit.removeChild( tit.firstChild );
    var span = tit.appendChild(document.createElement('span'));
    span.setAttribute('class','rname');
    span.appendChild(document.createTextNode(room['label']+' '));
    span = tit.appendChild(document.createElement('span'));
    span.setAttribute('class','rdesc');
    span.appendChild(document.createTextNode('('+room['sx']+' x '+room['sy']+' x '+room['sz']+' m'));
    var sup = span.appendChild(document.createElement('sup'));
    sup.appendChild(document.createTextNode('3'));
    span.appendChild(document.createTextNode(', T'));
    sup = span.appendChild(document.createElement('sub'));
    sup.appendChild(document.createTextNode('60'));
    var soundscape = '';
    if( room.ambientsound.length > 0 )
	soundscape = ', sound scape';
    span.appendChild(document.createTextNode(': '+room['t60'].toFixed(2)+' s'+soundscape+')'));
    if( room['private'] )
	span.appendChild(document.createTextNode(' private'));
    else{
	if( room['group'].length==0 )
	    span.appendChild(document.createTextNode(' public'));
	else
	    span.appendChild(document.createTextNode(' group \''+room['group']+'\''));
    }
    if( room.owner.length>0 )
	span.appendChild(document.createTextNode(', managed by '+room.owner));
    if( room['editable'] )
	span.appendChild(document.createTextNode(', acoustics can be changed'));
    var srvjit = Number(room['srvjit']);
    if( srvjit>0 ){
	var sjspan = span.appendChild(document.createElement('span'));
	sjspan.setAttribute('class','srvjit');
	if( srvjit<1 ){
	    sjspan.appendChild(document.createTextNode('***'));
	}else{
	    if( srvjit<5 ){
		sjspan.appendChild(document.createTextNode('**'));
	    }else{
		sjspan.appendChild(document.createTextNode('*'));
	    }       
	}
	span.appendChild(document.createTextNode('(jitter '+srvjit.toFixed(1)+' ms)'));
    }
    //span.appendChild(document.createTextNode(' ('+room.id+')'));
    var memb = document.getElementById(room.id+':members');
    while( memb.firstChild ) memb.removeChild(memb.firstChild);
    var senders = memb.appendChild(document.createElement('span'));
    senders.setAttribute('class','roomsender');
    var listeners = memb.appendChild(document.createElement('span'));
    listeners.setAttribute('class','roomlistener');
    for( const chair in room.roomdev ){
	var dev = room.roomdev[chair];
	var mem = document.createElement('span');
	if( dev.issender ){
	    senders.appendChild(mem);
	    senders.appendChild(document.createTextNode(' '));
	}else{
	    listeners.appendChild(mem);
	    listeners.appendChild(document.createTextNode(' '));
	}
	var tagsuffix = 'member';
	if( !dev.issender )
	    tagsuffix = 'listener';
	var bclass = 'psv';
	if( dev.isactive )
	    bclass = 'act';
	bclass = bclass + tagsuffix;
	mem.setAttribute('class',bclass);
	var latdisp = '';
	if( device.chair != chair ){
	    if( device.peer2peer && dev.peer2peer ){
		// display peer2peer latency:
		const latkey = chair + '-' + device.chair;
		if( room.lat.hasOwnProperty(latkey) ){
		    // only display values from last 15 minutes:
		    if( room.now-room.lat[latkey].access < 900 ){
			const lat = 0.5*Number(room.lat[latkey].lat)+Number(dev.jittersend)+Number(device.jitterreceive)+10;
			latdisp = ' '+lat.toFixed(1)+'ms ';
		    }
		}
	    }else{
		// display latency via server:
		const latkey1 = chair + '-200';
		const latkey2 = device.chair + '-200';
		if( room.lat.hasOwnProperty(latkey1) && room.lat.hasOwnProperty(latkey2) && (dev.id != device.id) ){
		    // only display values from last 15 minutes:
		    if( (room.now-room.lat[latkey1].access < 900) && (room.now-room.lat[latkey2].access < 900) ){
			const lat = 0.5*(Number(room.lat[latkey1].lat)+Number(room.lat[latkey2].lat))+Number(dev.jittersend)+Number(device.jitterreceive)+10;
			latdisp = ' '+lat.toFixed(1)+'ms ';
		    }
		}
	    }
	}
	//mem.appendChild(document.createTextNode(device.chair));
	var mtype = 'span';
	if( (dev.id != device.id) && room.entered && dev.issender && device.issender )
	    mtype = 'a';
	if ( dev.id == device.id ){
	    if( device.issender )
		mem.setAttribute('style','border: 3px solid #000000;');
	    else
		mem.setAttribute('style','border: 2px solid #606060;');
	}
	var memlink = mem.appendChild(document.createElement(mtype));
	memlink.setAttribute('class',bclass);
	if( mtype == 'a')
	    memlink.setAttribute('href','?swapdev='+encodeURI(dev.id));
	var lab = dev.label;
	if( lab.length==0 )
	    lab = dev.id;
	memlink.appendChild(document.createTextNode( escapeHtml(lab) ));
	if( room.entered ){
	    if( dev.peer2peer )
		latdisp = 'p2p ' + latdisp;
	    else
		latdisp = 'srv ' + latdisp;
	}
	if( latdisp.length > 0 ){
	    var span = mem.appendChild(document.createElement('span'));
	    span.setAttribute('class','latency');
	    span.appendChild(document.createTextNode(latdisp));
	}
	if( (room.owner == device.owner) || (dev.owner == device.owner) ){
	    var kick = mem.appendChild(document.createElement('input'));
	    kick.setAttribute('value','X');
	    kick.setAttribute('class','kick');
	    kick.setAttribute('type','button');
	    kick.setAttribute('title','Kick this device out of my room.');
	    kick.setAttribute('onclick','location.href=\'?kick='+encodeURI(dev.id)+'\';');
	}
    }
    var ctl = document.getElementById(room.id+':ctl');
    while( ctl.firstChild ) ctl.removeChild( ctl.firstChild );
    if( device.id ){
	if( room.entered ) {
	    var a = ctl.appendChild(document.createElement('a'));
	    a.setAttribute('href','?enterroom=');
	    a.appendChild(document.createTextNode('leave room'));
	    ctl.appendChild(document.createTextNode(' '));
	    a = ctl.appendChild(document.createElement('a'));
	    if( room.lock ){
		a.setAttribute('href','?lockroom='+encodeURI(room.id)+'&lck=0');
		a.appendChild(document.createTextNode('unlock room'));
	    }else{
		a.setAttribute('href','?lockroom='+encodeURI(room.id)+'&lck=1');
		a.appendChild(document.createTextNode('lock room'));
	    }
	} else {
	    if( room.lock ){
		ctl.appendChild(document.createTextNode('room is locked.'));
	    }else{
		var a = ctl.appendChild(document.createElement('a'));
		a.setAttribute('href','?enterroom='+encodeURI(room.id));
		a.appendChild(document.createTextNode('enter'));
	    }
	}
    }
    if( (device.owner == room.owner)||(room.editable && room.entered) ){
	// my room, provide settings box:
	ctl.appendChild(document.createTextNode(' '));
	if( device.owner == room.owner ){
	    if( room.roomdev.length>0 ){
		var a = ctl.appendChild(document.createElement('a'));
		a.setAttribute('href','?clearroom='+encodeURI(room.id));
		a.appendChild(document.createTextNode('kick all'));
	    }
	}
	var rp = document.getElementById(room.id+':cfg');
	var tog = ctl.appendChild(document.createElement('input'));
	tog.setAttribute('type','button');
	tog.setAttribute('class','roomsettingstoggle uibutton');
	tog.setAttribute('onclick','toggledisplay("'+room.id+':cfg","room settings");');
	tog.setAttribute('value','show room settings');
    }
    if( !roomvaluechanged ){
	for (const [key, value] of Object.entries(room)) {
	    var inp = document.getElementById(room.id+':cfg:'+key);
	    if( inp ){
		switch( inp.type ){
		case 'checkbox' :
	    	    if( value )
			inp.setAttribute('checked','');
		    else
			inp.removeAttribute('checked');
		    break;
		case 'number' :
		case 'text' :
		    inp.value = value;
		    break;
		case 'select-one' :
		    for( var k=0;k<inp.options.length;k++){
			inp.options[k].selected = inp.options[k].value==value;
		    }
		    break;
		default:
		    console.log('key '+key+' type: '+inp.type);
		}
	    }
	}
    }
}

function secondsToTime(inputSeconds) {
    const secondsInAMinute = 60;
    const secondsInAnHour  = 60 * secondsInAMinute;
    const secondsInADay    = 24 * secondsInAnHour;
    // extract days
    var days = Math.floor(inputSeconds / secondsInADay);
    // extract hours
    var hourSeconds = inputSeconds % secondsInADay;
    var hours = Math.floor(hourSeconds / secondsInAnHour);
    // extract minutes
    var minuteSeconds = hourSeconds % secondsInAnHour;
    var minutes = Math.floor(minuteSeconds / secondsInAMinute);
    // extract the remaining seconds
    var remainingSeconds = minuteSeconds % secondsInAMinute;
    var seconds = Math.ceil(remainingSeconds);
    // return the final array
    var obj = Object;
    obj.d = days.toFixed(0);
    obj.h = hours.toFixed(0);
    obj.m = minutes.toFixed(0);
    obj.s = seconds.toFixed(0);
    return obj;
}

function numage2str( nage )
{
    var d = secondsToTime( nage );
    var age = '';
    if( nage > 3600*24*365*40 )
	return 'never';
    if( nage >= 3600*24 )
	age = age + d.d + 'd';
    if( (nage >= 3600) && (nage < 7*3600*24) )
	age = age + d.h + 'h';
    if( (nage >= 60) && (nage < 3600*24) )
	age = age + d.m + '\'';
    if( (nage >= 0) && (nage < 3600) )
	age = age + d.s + '"';
    if( nage < 0 )
	age = nage + 's';
    return age;
}

function update_deviceuser( user, device, owned_devices )
{
    var p = document.getElementById('deviceuser');
    while( p.firstChild ) p.removeChild(p.firstChild);
    p.appendChild(document.createTextNode('You are logged in as user '));
    p.appendChild(document.createElement('b')).appendChild(document.createTextNode(user));
    if( device.id.length==0 ){
	p.appendChild(document.createTextNode(' with no device.'));
    }else{
	var dclass = 'psvmember';
	var state = '';
	var lastseen = '';
	var otherdev = '';
	if( device.age < 20 ){
	    dclass = 'actmember';
	    state = 'active';
	}else{
	    lastseen = ', inactive since '+numage2str(device.age);
	    var oact = false;
	    for( const od in owned_devices){
		if( owned_devices[od].age<20 )
		    oact = true;
	    }
	    if( oact )
		otherdev = ' You own active devices - please check the device selector below to access them.';
	}
	p.appendChild(document.createTextNode(' with '+state+' device '));
	var span = p.appendChild(document.createElement('span'));
	span.setAttribute('class',dclass);
	span.appendChild(document.createElement('b')).appendChild(document.createTextNode(device.id+' ('+device.label+')' ));
	p.appendChild(document.createTextNode(lastseen+'.'+otherdev));
	if( device.age < 20 ){
	    if( (device.bandwidth.tx>0)||(device.bandwidth.rx>0) ){
		var txstr;
		if( device.bandwidth.tx >= 100000 )
		    txstr = (0.000001*device.bandwidth.tx).toFixed(2)+' MBps';
		else
		    txstr = (0.001*device.bandwidth.tx).toFixed(2)+' kBps';
		var rxstr;
		if( device.bandwidth.rx >= 100000 )
		    rxstr = (0.000001*device.bandwidth.rx).toFixed(2)+' MBps';
		else
		    rxstr = (0.001*device.bandwidth.rx).toFixed(2)+' kBps';
		p.appendChild(document.createTextNode(' sending: '+txstr+', receiving: '+rxstr));
	    }
	    if( device.cpuload > 0 ){
		p.appendChild(document.createTextNode(' CPU load: '+(100*device.cpuload).toFixed(1)+'%'));
	    }
	}
    }
    // update device error:
    var deverr = document.getElementById('deverror');
    if( deverr ){
	while( deverr.firstChild ) deverr.removeChild(deverr.firstChild);
	if( (device.message.length>0) && (device.age<3600) ){
	    deverr.setAttribute('style','display: block;');
	    var b = deverr.appendChild(document.createElement('b'));
	    b.appendChild(document.createTextNode('Device error:'));
	    deverr.appendChild(document.createElement('br'));
	    deverr.appendChild(document.createTextNode(device.message));
	    if( device.message.includes('Unable to connect to the JACK server')){
		deverr.appendChild(document.createElement('br'));
		deverr.appendChild(document.createTextNode('Is your sound card connected and configured correctly?'));
	    }
	}else{
	    deverr.setAttribute('style','display: none;');
	}
    }
    // update device selector:
    var devsel = document.getElementById('deviceselector');
    while( devsel.options.length > 0 )
	devsel.remove(0);
    var opt = document.createElement('option');
    opt.value = '';
    opt.text = '-- please select a device --';
    devsel.add(opt);
    for( const od in owned_devices){
	var act = '';
	if( owned_devices[od].age<20 )
	    act = ' *active*';
	opt = document.createElement('option');
	opt.value = od;
	opt.text = od+' ('+owned_devices[od].label+')'+act;
	opt.selected = od==device.id;
	devsel.add(opt);
    }
    // update webmixer link:
    var webm = document.getElementById('webmixerlink');
    if( webm ){
	while( webm.firstChild ) webm.removeChild(webm.firstChild);
	var mixer = device.host;
	if( mixer.length = 0 )
	    mixer = device.localip;
	if( (device.age < 20) && (mixer.length > 0) ){
            // device is active and we know the host name:
            webm.setAttribute('style','display: block;');
            var a = webm.appendChild(document.createElement('a'));
            a.setAttribute('href','http://'+mixer+':8080/');
            a.setAttribute('target','_blank');
            a.setAttribute('rel','noopener noreferrer');
            a.setAttribute('class','mixer');
            webm.setAttribute('class','mixer');
            a.appendChild(document.createTextNode('open mixer'));
            webm.appendChild(document.createTextNode(' (works only when your browser is in the same network as your device)'));
	}else{
            webm.setAttribute('style','display: none;');
	}
    }
}

function updaterooms()
{
    var droom=document.getElementById('roomlist');
    if( droom ){
	var droomrm=document.getElementById('roomlistremove');
	if( droomrm )
	    droomrm.remove(droomrm);
	let request = new XMLHttpRequest();
	request.onload = function() {
	    var data = JSON.parse(request.response, (key,value)=>
				  {
				      if( value == "0" ) return 0;
				      if( value == "0.000000" ) return 0.0;
				      return value;
				  }
				 );
	    var user = data.user;
	    var rooms = data.rooms;
	    var device = data.device;
	    var owned_devices = data.owned_devices;
	    // update device display:
	    update_deviceuser( user, device, owned_devices );
	    // delete unused rooms:
	    for( let k=droom.children.length-1;k>=0;k--){
		if( rooms.find(room=>room.id==droom.children[k].id)===undefined)
		    droom.removeChild(droom.children[k]);
	    }
	    for( var k=0;k<rooms.length;k++){
		// room div:
		update_room( device, rooms[k], droom );
	    }
	}
	request.open('GET', 'rest.php?getrooms');
	request.reponseType = 'json';
	request.send();
    }
}

var tstart;
var timer;
var timer10;

function everysecond(){
    var el=document.getElementsByClassName("timedisplay");
    for( var k=0,len=el.length|0;k<len;k=k+1|0 ){
	while (el[k].firstChild) {el[k].removeChild(el[k].firstChild);}
	el[k].appendChild(document.createTextNode(Math.floor(0.001*(Date.now()-tstart))));
    }
}

function everytenseconds(){
    updaterooms();
}

function starttimer(){
    tstart=Date.now();
    timer=setInterval(everysecond,1000);
    everytenseconds();
    timer10=setInterval(everytenseconds,10000);
}

function dispvaluechanged(id){
    var savebutton;
    if( id == "valuechanged" )
	savebutton=document.getElementById('devsettingssave');
    else{
	savebutton=document.getElementById('roomsettingssave');
	roomvaluechanged = true;
    }
    if( savebutton )
	savebutton.style.border="5px solid #aa0000";
    var el=document.getElementsByClassName(id);
    for( var k=0,len=el.length|0;k<len;k=k+1|0 ){
        while (el[k].firstChild) {
	    el[k].removeChild(el[k].firstChild);
	}
        el[k].appendChild(document.createTextNode(" Press Save to apply changes."));
    }
}
