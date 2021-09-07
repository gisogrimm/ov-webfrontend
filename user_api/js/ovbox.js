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

function toggle_ovnavh(){
    var x=document.getElementById("ovnavh");
    if( x ){
	if((x.style.display.length==0) || (x.style.display==="none")){
	    x.style.display="block";
	}else{
	    x.style.display="none";
	}
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

function set_displayclass( id, value ){
    var x=document.getElementsByClassName(id);
    for(var k=0;k<x.length;k++){
	if( value )
	    x[k].style.display="block";
	else
	    x[k].style.display="none";
    }
}

function setmetro( name, value )
{
    let request = new XMLHttpRequest();
    request.open('GET', 'rest.php?metro'+name+'='+value);
    request.send();
    if( name == 'active' ){
	var x = document.getElementById('metrocontrols');
	if( x ){
	    if( value )
		x.setAttribute('style','display: block;')
	    else
		x.setAttribute('style','display: none;')
	}
    }
}

function rest_setval( name, value )
{
    let request = new XMLHttpRequest();
    request.open('GET', 'rest.php?'+name+'='+value);
    request.send();
}

function rest_setval_reload( name, value )
{
    let request = new XMLHttpRequest();
    request.onload = function() {
	location.reload();
    }
    request.open('GET', 'rest.php?'+name+'='+value, true);
    request.send();
}

function rest_setval_post( name, value )
{
    let request = new XMLHttpRequest();
    request.open('POST', '/rest.php', true);
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    request.send(name+'='+value);
}

function rest_setval_post_reload( name, value )
{
    let request = new XMLHttpRequest();
    request.onload = function() {
	location.reload();
    }
    request.open('POST', '/rest.php', true);
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    request.send(name+'='+value);
}

function rest_set_devprop( name, value )
{
    let request = new XMLHttpRequest();
    request.open('POST', '/rest.php', true);
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    if( typeof value === "boolean" )
	request.send('setdevpropbool='+name+'&'+name+'='+value);
    else if( typeof value === "number" )
	request.send('setdevpropfloat='+name+'&'+name+'='+value);
    else
	request.send('setdevprop='+name+'&'+name+'='+value);
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
    var bbed = eroom.appendChild(document.createElement('form'));
    bbed.setAttribute('id',id+':bulletinboard:editor');
    //bbed.setAttribute('class','devprop');
    bbed.setAttribute('method','POST');
    bbed.setAttribute('style','display: none;');
    var inp = bbed.appendChild(document.createElement('input'));
    inp.setAttribute('name','editbulletinboard');
    inp.setAttribute('value',id);
    inp.setAttribute('type','hidden');
    var inp = bbed.appendChild(document.createElement('textarea'));
    inp.setAttribute('name','bulletinboard');
    inp.setAttribute('rows','4');
    inp.setAttribute('cols','60');
    inp.setAttribute('class','bulletinboard');
    inp.appendChild(document.createTextNode(room.bulletinboard));
    var inp = bbed.appendChild(document.createElement('button'));
    inp.appendChild(document.createTextNode('Save'));
    var bulletinboard = eroom.appendChild(document.createElement('div'));
    bulletinboard.setAttribute('id',id+':bulletinboard');
    bulletinboard.setAttribute('class','bulletinboard');
    bulletinboard.setAttribute('style','display: none;');
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
    var inpx = add_input_to_form(form,id,'sx','number');
    inpx.setAttribute('step','0.1');
    inpx.setAttribute('min','0');
    inpx.setAttribute('max','300');
    inpx.setAttribute('value',room.sx);
    inpx.setAttribute('style','width: 50px;');
    var inpy = add_input_to_form(form,id,'sy','number');
    inpy.setAttribute('step','0.1');
    inpy.setAttribute('min','0');
    inpy.setAttribute('max','200');
    inpy.setAttribute('value',room.sy);
    inpy.setAttribute('style','width: 50px;');
    var inpz = add_input_to_form(form,id,'sz','number');
    inpz.setAttribute('step','0.1');
    inpz.setAttribute('min','0');
    inpz.setAttribute('max','100');
    inpz.setAttribute('value',room.sz);
    inpz.setAttribute('style','width: 50px;');
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
    // bulletin board:
    if( room.entered ){
	var tog = tit.appendChild(document.createElement('input'));
	tog.setAttribute('type','button');
	tog.setAttribute('class','roomsettingstoggle uibutton');
	tog.setAttribute('onclick','toggledisplay("'+room.id+':bulletinboard:editor","bulletin board editor");');
	tog.setAttribute('value','show bulletin board editor');
    }
    var bull = document.getElementById(room.id+':bulletinboard');
    while( bull.firstChild ) bull.removeChild( bull.firstChild );
    if( room.bulletinboard.length > 0 ){
	bull.appendChild(document.createTextNode(room.bulletinboard));
	bull.setAttribute('style','display: block;');
    }else{
	bull.setAttribute('style','display: none;');
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
	if( dev.issender && (!dev.senddownmix) ){
	    senders.appendChild(mem);
	    senders.appendChild(document.createTextNode(' '));
	}else{
	    listeners.appendChild(mem);
	    listeners.appendChild(document.createTextNode(' '));
	}
	var tagsuffix = 'member';
	if( !dev.issender )
	    tagsuffix = 'listener';
        if( dev.senddownmix )
            tagsuffix = 'downmix';
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
	if( (dev.id != device.id) && room.entered && dev.issender && device.issender && (!dev.senddownmix) )
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
	    if( (dev.numchannels > 1) && (!dev.senddownmix) )
		latdisp = dev.numchannels + 'c ' + latdisp;
	    latdisp = (dev.jackrate*0.001).toFixed(0) + 'k ' + latdisp;
            if( !dev.senddownmix ){
	        if( dev.peer2peer )
		    latdisp = 'p2p ' + latdisp;
	        else
		    latdisp = 'srv ' + latdisp;
            }
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
	if( room.lock ){
	    var lck = ctl.appendChild(document.createElement('img'));
	    lck.setAttribute('src','lock.svg');
	    lck.setAttribute('width','20px');
	    ctl.appendChild(document.createTextNode(' '));
	}
	if( room.entered ) {
	    var a = ctl.appendChild(document.createElement('a'));
	    a.setAttribute('href','?enterroom=');
	    a.setAttribute('class','roomctl');
	    a.appendChild(document.createTextNode('leave room'));
	    a = ctl.appendChild(document.createElement('a'));
	    a.setAttribute('class','roomctl');
	    if( room.lock ){
		a.setAttribute('href','?lockroom='+encodeURI(room.id)+'&lck=0');
		a.appendChild(document.createTextNode('unlock room'));
	    }else{
		a.setAttribute('href','?lockroom='+encodeURI(room.id)+'&lck=1');
		a.appendChild(document.createTextNode('lock room'));
	    }
	    a = ctl.appendChild(document.createElement('a'));
	    a.setAttribute('class','roomctl');
	    a.setAttribute('href','sessionmap.php');
	    a.appendChild(document.createTextNode('map'));
	    a = ctl.appendChild(document.createElement('a'));
	    a.setAttribute('class','roomctl');
	    a.setAttribute('href','sessionstat.php');
	    a.appendChild(document.createTextNode('statistics'));
	} else {
	    if( room.lock ){
		ctl.appendChild(document.createTextNode('room is locked.'));
	    }else{
		var a = ctl.appendChild(document.createElement('a'));
		a.setAttribute('class','roomctl');
		a.setAttribute('href','?enterroom='+encodeURI(room.id));
		a.appendChild(document.createTextNode('enter'));
	    }
	}
    }
    if( (device.owner == room.owner)||(room.editable && room.entered) ){
	// my room, provide settings box:
	if( device.owner == room.owner ){
	    var numdevs = 0;
	    for( const dev in room.roomdev)
		numdevs++;
	    if( numdevs>0 ){
		var a = ctl.appendChild(document.createElement('a'));
		a.setAttribute('href','?clearroom='+encodeURI(room.id));
		a.appendChild(document.createTextNode('kick all'));
		a.setAttribute('class','roomctl');
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

function update_devicestatus( user, device, owned_devices )
{
    var devstat = document.getElementById('devstatus');
    while( devstat.firstChild ) devstat.removeChild(devstat.firstChild);
    if( device.id.length==0 ){
	devstat.appendChild(document.createTextNode('No device is linked to this account.'));
    }else{
	var dclass = 'psvmember';
	//var state = '';
	var lastseen = '';
	var otherdev = '';
        if( device.firmwareupdate ){
	    if( device.age > 1800 ){
                lastseen = ' Firmware update pending. ';
            }else{
                lastseen = ' Firmware updating - please do not disconnect from power or internet. ';
            }
        }
        var devver = document.getElementById('devfirmwareversion');
        while( devver.firstChild ) devver.removeChild(devver.firstChild);
        devver.appendChild(document.createTextNode(device.version+lastseen));
	if( device.age < 20 ){
	    dclass = 'actmember';
	    //state = 'active';
	}else{
	    lastseen = lastseen+' inactive since '+numage2str(device.age)+'.';
	    var oact = false;
	    for( const od in owned_devices){
		if( owned_devices[od].age<20 )
		    oact = true;
	    }
	    if( oact )
		otherdev = ' You own active devices - please check the device selector above to access them.';
	}
	devstat.appendChild(document.createTextNode(lastseen+otherdev));
	if( device.age < 20 ){
	    if( device.bandwidth && ((device.bandwidth.tx>0)||(device.bandwidth.rx>0)) ){
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
		devstat.appendChild(document.createTextNode(' sending: '+txstr+', receiving: '+rxstr));
	    }
	    if( device.cpuload && (device.cpuload > 0) ){
		devstat.appendChild(document.createTextNode(' CPU load: '+(100*device.cpuload).toFixed(1)+'%'));
	    }
	}
	if( device.useproxy && (device.proxyip.length>0))
	    devstat.appendChild(document.createTextNode(' proxy: '+device.proxyip));
	if( device.isproxy )
	    devstat.appendChild(document.createTextNode(' offering proxy service'));
	if( device.lastfrontendconfig && device.lastfrontendconfig.ui){
	    devstat.appendChild(document.createElement('br'));
	    devstat.appendChild(document.createTextNode('Currently registered at '));
	    var ahref = devstat.appendChild(document.createElement('a'));
	    ahref.setAttribute('href',device.lastfrontendconfig.ui);
	    ahref.appendChild(document.createTextNode(device.lastfrontendconfig.ui));
	}
	//console.log(device.lastfrontendconfig);
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
    if( devsel ){
	while( devsel.options.length > 0 )
	    devsel.remove(0);
	var opt = document.createElement('option');
	opt.value = '';
	opt.text = '-- please select a device --';
	devsel.add(opt);
	for( const od in owned_devices){
	    var act = '';
	    if( (owned_devices[od].age<20) && (od!=device.id))
		act = ' *active*';
	    opt = document.createElement('option');
	    opt.value = od;
	    opt.text = od+' ('+owned_devices[od].label+')'+act;
	    opt.selected = od==device.id;
	    devsel.add(opt);
	}
	if( device.age < 20 )
	    devsel.setAttribute('class','actmember');
	else
	    devsel.setAttribute('class','psvmember');
    }
    // update webmixer link:
    var webm = document.getElementById('webmixerlink');
    if( webm ){
	while( webm.firstChild ) webm.removeChild(webm.firstChild);
	// use IP address for mixer if possible:
	var mixer = device.localip;
	if( mixer.length = 0 )
	    mixer = device.host;
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
    var presetindicator = document.getElementById('presetindicator');
    if( presetindicator ){
	while( presetindicator.firstChild ) presetindicator.removeChild(presetindicator.firstChild);
	if( device.preset.length > 0 ){
	    presetindicator.appendChild(document.createTextNode(device.preset));
	    presetindicator.setAttribute('class','presetspan presetact');
	}else{
	    presetindicator.setAttribute('class','');
	    var els=document.getElementsByClassName("presetact");
	    for( const el in els ){
		if( els.item(el) ){
		    var cl = els.item(el).getAttribute('class');
		    els.item(el).setAttribute('class',cl.replace('presetact',''));
		}
	    }
	}
    }
    //if(!empty($dprop['preset'])){
    //	$pres->appendChild($doc->createTextNode($dprop['preset']));
    //	$pres->setAttribute('class','presetspan presetact');
    //}
}

function update_unclaimed( user, unclaimed_devices )
{
    var p = document.getElementById('devclaim');
    while( p.firstChild ) p.removeChild(p.firstChild);
    if( unclaimed_devices.length == 0 )
	p.setAttribute('style','display:none;');
    else{
	p.setAttribute('style','display:block;');
	p.appendChild(document.createTextNode('Unclaimed active devices exist. If this is your device, and it is active now, you may claim it by clicking on the device id:'));
	p.appendChild(document.createElement('br'));
        for( var k=0;k<unclaimed_devices.length;k++){
	    var form=p.appendChild(document.createElement('form'));
	    form.setAttribute('style','display:inline;');
	    var inp=form.appendChild(document.createElement('input'));
	    inp.setAttribute('type','hidden');
	    inp.setAttribute('name','claim');
	    inp.setAttribute('value',unclaimed_devices[k]);
	    form.appendChild(document.createElement('button')).appendChild(document.createTextNode(unclaimed_devices[k]));
        }
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

function hexval(c){
    if( c < 0 )
	return '00';
    if( c < 16 )
	return '0'+c.toString(16);
    if( c < 256 )
	return c.toString(16);
    return 'ff';
}

function lat2rgb(lat,good,bad){
    const delta=bad-good;
    const r=Math.round(220*(2*lat/delta));
    const g=Math.round(200*(2*(bad-lat)/delta));
    return "#"+hexval(r)+hexval(g)+'00';
}

function tab_header( tab, data ){
    var tr=tab.appendChild(document.createElement('tr'));
    var td=tr.appendChild(document.createElement('td'));
    td=tr.appendChild(document.createElement('td'));
    var numch = 0;
    for( const chair in data.chairs )
	numch++;
    td.setAttribute('colspan',numch);
    td.setAttribute('class','statcell statcellsend');
    td.appendChild(document.createTextNode('sender'));
    tr=tab.appendChild(document.createElement('tr'));
    td=tr.appendChild(document.createElement('td'));
    td.appendChild(document.createTextNode('receiver'));
    td.setAttribute('class','statcellrec');
    for( const chair in data.chairs ){
	const dev=data.chairs[chair];
	if( dev && data.stats[dev] ){
	    var td=tr.appendChild(document.createElement('td'));
	    td.setAttribute('class','statcell statcellsend');
	    if( data.versions[dev] < 0 )
		td.appendChild(document.createTextNode('('+chair+')'));
	    else
		td.appendChild(document.createTextNode(chair));
	}
    }
}

function create_tab_stat(div,title,data,header=true){
    var sec_ping=div.appendChild(document.createElement('div'));
    sec_ping.setAttribute('class','ovsection');
    var h_ping=sec_ping.appendChild(document.createElement('div'));
    h_ping.setAttribute('class','ovsectiontitle devproptitle');
    h_ping.appendChild(document.createTextNode(title));
    var tab = sec_ping.appendChild(document.createElement('table'));
    if( header )
	tab_header( tab, data );
    return tab;
}

function create_row_stat(tab,chair,data){
    const dev=data.chairs[chair];
    var tr=tab.appendChild(document.createElement('tr'));
    var td=tr.appendChild(document.createElement('td'));
    if( data.versions[dev] < 0 )
	td.appendChild(document.createTextNode('('+chair+' '+data.names[dev]+')'));
    else
	td.appendChild(document.createTextNode(chair+' '+data.names[dev]));
    td.setAttribute('class','statcellrec');
    return tr;
}

function data2mat( data, category, measure )
{
    var mat = [];
    for( const chair in data.chairs ){
	const dev=data.chairs[chair];
	if( dev && data.stats[dev] ){
	    for( const chx in data.chairs ){
		if( data.stats[dev][chx] ){
		    mat.push(data.stats[dev][chx][category][measure]);
		}else{
		    mat.push(NaN);
		}
	    }
	}
    }
    return mat;
}

function matelem(mat,x,y,n)
{
    return mat[x+n*y];
}

function sqr(x)
{
    return x*x;
}

function get_optimal_receiver_jitter( data, category )
{
    var chidx = new Object();
    var idx = 0;
    var chairidx = new Object();
    for( const chair in data.chairs ){
	chidx[chair] = idx;
	chairidx[idx] = chair;
	idx++;
    }
    const dp99 = data2mat(data,category,'p99');
    const dmin = data2mat(data,category,'min');
    const mat_jitter = dp99.map(function (num,idx) { return num-dmin[idx];});
    var vjitrec = {};
    for( const chair in data.chairs ){
	var jitrec = 1000;
	for( var k=data.n*chidx[chair]; k<data.n*(chidx[chair]+1);k++){
	    if( mat_jitter[k] > 0 )
		jitrec = Math.min(jitrec,mat_jitter[k]);
	}
	vjitrec[chair] = jitrec;
    }
    var vjitsend = {};
    for( const chair in data.chairs ){
	var jitsend = 0;
	for( var k=0;k<data.n;k++){
	    if( matelem(mat_jitter,chidx[chair],k,data.n) > 0 ){
		var sq = Math.sqrt(sqr(matelem(mat_jitter,chidx[chair],k,data.n))-sqr(vjitrec[chairidx[k]]));
		jitsend = Math.max(jitsend,sq);
	    }
	}
	vjitsend[chair] = Math.ceil(Math.sqrt(sqr(jitsend)+sqr(data.fragsize[data.chairs[chair]])));
    }
    for( const chair in vjitsend ){
	vjitrec[chair] = Math.ceil(Math.sqrt(sqr(vjitrec[chair])+sqr(data.fragsize[data.chairs[chair]])));
    }
    return {'rec':vjitrec,'send':vjitsend};
}

function update_sessionmap(div)
{
    let request = new XMLHttpRequest();
    request.onload = function() {
	var svg = request.responseXML;
	while( div.firstChild ) div.removeChild(div.firstChild);
	var img = div.appendChild(svg.rootElement.cloneNode(true));
        var maxw = div.clientWidth;
        if( 0.6*top.innerHeight < maxw ){
            maxw = 0.6*top.innerHeight;
            div.setAttribute('style','width:'+maxw.toString()+'px;');
        }else{
            div.removeAttribute('style');
        }
    }
    request.open('GET', 'sessionsvg.php');
    request.reponseType = 'svg';
    request.send();
}

function update_sessionstat(div)
{
    let request = new XMLHttpRequest();
    request.onload = function() {
	var data = JSON.parse(request.response, (key,value)=>
			      {
				  if( value == "0" ) return 0;
				  return value;
			      }
			     );
	while( div.firstChild ) div.removeChild(div.firstChild);
	if( data && data.stats && (data.room.length>0)){
	    var h=div.appendChild(document.createElement('h3'));
	    h.appendChild(document.createTextNode('Session '+data.room));
	    const modes = ['cur','p2p','srv','loc']
	    for( const mode in modes){
		var show = false;
		for( const chair in data.chairs ){
		    const dev=data.chairs[chair];
		    if( dev && data.stats[dev] ){
			for( const chx in data.chairs ){
			    if( data.stats[dev][chx] && data.stats[dev][chx][modes[mode]])
				if( data.stats[dev][chx][modes[mode]].received > 0 )
				    show = true;
			}
		    }
		}
		if(show){
		    {
			var tab = create_tab_stat(div,'Suggestions '+modes[mode],data,false);
			const suggest = get_optimal_receiver_jitter(data,modes[mode]);
			var tr = tab.appendChild(document.createElement('tr'));
			td = tr.appendChild(document.createElement('td'));
			td = tr.appendChild(document.createElement('td'));
			td.setAttribute('class','statcell');
			td.appendChild(document.createTextNode('rec'));
			td = tr.appendChild(document.createElement('td'));
			td.appendChild(document.createTextNode('send'));
			td.setAttribute('class','statcell');
			for( const chair in data.chairs ){
			    var tr=create_row_stat(tab,chair,data);
			    td = tr.appendChild(document.createElement('td'));
			    td.setAttribute('class','statcell');
			    td.appendChild(document.createTextNode(suggest.rec[chair]));
			    td = tr.appendChild(document.createElement('td'));
			    td.setAttribute('class','statcell');
			    td.appendChild(document.createTextNode(suggest.send[chair]));
			    td = tr.appendChild(document.createElement('td'));
			    td.setAttribute('class','statcell');
			    if( data.p2p[data.chairs[chair]] )
				td.appendChild(document.createTextNode('p2p'));
			    else
				td.appendChild(document.createTextNode('srv'));
			}
		    }
		    var tab_ping=create_tab_stat(div,'Median ping times '+modes[mode], data);
		    var tab_jitter=create_tab_stat(div,'Jitter '+modes[mode], data);
		    for( const chair in data.chairs ){
			const dev=data.chairs[chair];
			if( dev && data.stats[dev] ){
			    // ping:
			    var tr=create_row_stat(tab_ping,chair,data);
			    for( const chx in data.chairs ){
				var pt=-1;
				if( data.stats[dev][chx] )
				    pt=data.stats[dev][chx][modes[mode]].median;
				var td=tr.appendChild(document.createElement('td'));
				td.setAttribute('class','statcell');
				if( pt > 0){
				    td.appendChild(document.createTextNode(pt.toFixed(1)+'ms'));
				    td.setAttribute('style','background-color:'+lat2rgb(pt,0,60));
				}else{
				    //td.appendChild(document.createTextNode('---'));
				    td.setAttribute('style','background-color: #AAAAAA');
				}
			    }
			    // jitter:
			    var tr=create_row_stat(tab_jitter,chair,data);
			    for( const chx in data.chairs ){
				var pt=-1;
				if( data.stats[dev][chx] )
				    pt=data.stats[dev][chx][modes[mode]].p99-
				    data.stats[dev][chx][modes[mode]].min;
				var td=tr.appendChild(document.createElement('td'));
				td.setAttribute('class','statcell');
				if( pt > 0){
				    td.appendChild(document.createTextNode(pt.toFixed(1)+'ms'));
				    td.setAttribute('style','background-color:'+lat2rgb(pt,0,15));
				}else{
				    //td.appendChild(document.createTextNode('---'));
				    td.setAttribute('style','background-color: #AAAAAA');
				}
			    }
			}
		    }
		}
	    }
	    var tab=create_tab_stat(div,'Package loss', data);
	    for( const chair in data.chairs ){
		const dev=data.chairs[chair];
		if( dev && data.stats[dev] ){
		    // ping:
		    var tr=create_row_stat(tab,chair,data);
		    for( const chx in data.chairs ){
			var pt=-1;
			if( data.stats[dev][chx] && data.stats[dev][chx].packages ){
			    const p = data.stats[dev][chx].packages;
			    if( p.received+p.lost>0 ){
				pt=100.0*p.lost/(p.received+p.lost);
			    }
			}
			var td=tr.appendChild(document.createElement('td'));
			td.setAttribute('class','statcell');
			if( pt >= 0){
			    td.appendChild(document.createTextNode(pt.toFixed(2)+'%'));
			    td.setAttribute('style','background-color:'+lat2rgb(pt,0,0.2));
			}else{
			    //td.appendChild(document.createTextNode('---'));
			    td.setAttribute('style','background-color: #AAAAAA');
			}
		    }
		}
	    }
	    var tab=create_tab_stat(div,'Sequence error/corrected', data);
	    for( const chair in data.chairs ){
		const dev=data.chairs[chair];
		if( dev && data.stats[dev] ){
		    var tr=create_row_stat(tab,chair,data);
		    for( const chx in data.chairs ){
			var pt=-1;
			var pt2=-1;
			if( data.stats[dev][chx] && data.stats[dev][chx].packages ){
			    const p = data.stats[dev][chx].packages;
			    if( p.received+p.lost>0 ){
				pt = p.seqerr;
				pt2 = p.seqrecovered;
			    }
			}
			var td=tr.appendChild(document.createElement('td'));
			td.setAttribute('class','statcell');
			if( pt >= 0){
			    td.appendChild(document.createTextNode(pt + '/' + pt2));
			    td.setAttribute('style','background-color:'+lat2rgb(pt,0,2));
			}else{
			    td.setAttribute('style','background-color: #AAAAAA');
			}
		    }
		}
	    }
	}else{
	    div.appendChild(document.createTextNode('No data available.'));
	}
    }
    request.open('GET', 'rest.php?getsessionstat');
    request.reponseType = 'json';
    request.send();
}

function everytenseconds()
{
    var droom=document.getElementById('roomlist');
    var droomrm=document.getElementById('roomlistremove');
    var devstat = document.getElementById('devstatus');
    var devclaim = document.getElementById('devclaim');
    var phpdeviceid = document.getElementById('phpdeviceid');
    var sessionstat = document.getElementById('sessionstat');
    var sessionmap = document.getElementById('sessionmap');
    if( droom || devstat || devclaim ){
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
	    var unclaimed_devices = data.unclaimed_devices;
	    if( phpdeviceid ){
		if( phpdeviceid.value != device.id)
		    location.reload();
	    }
	    if( devstat )
		// update device display:
		update_devicestatus( user, device, owned_devices );
	    if( devclaim )
		update_unclaimed( user, unclaimed_devices );
	    if( droom ){
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
	    if( sessionstat )
		update_sessionstat(sessionstat);
	    if( sessionmap )
		update_sessionmap(sessionmap);
	}
	request.open('GET', 'rest.php?getrooms');
	request.reponseType = 'json';
	request.send();
    }
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

function dispvaluechanged_id(id){
    var savebutton=document.getElementById(id);
    if( savebutton )
	savebutton.style.border="5px solid #aa0000";
}

function update_jack_rate( rate ){
    if( rate < 32000 )
	document.getElementById('jackplugdev').checked = true;
    if( rate > 32000 )
	document.getElementById('jackplugdev').checked = false;
    document.getElementById('jackrate').value = rate;
    document.getElementById('jackperiod').value = 16*Math.floor(0.002*rate/16);
}

function create_preset(){
    let request = new XMLHttpRequest();
    request.onload = function() {
	location.reload();
    }
    request.open('GET', 'rest.php?devpresetsave=' + document.getElementById('savepresetname').value);
    request.send();
}

function load_preset( preset ){
    let request = new XMLHttpRequest();
    request.onload = function() {
	location.reload();
    }
    request.open('GET', 'rest.php?devpresetload=' + preset);
    request.send();
}

function rm_preset( preset ){
    if( confirm('Really delete preset "'+preset+'"?') ){
	let request = new XMLHttpRequest();
	request.onload = function() {
	    location.reload();
	}
	request.open('GET', 'rest.php?devpresetrm=' + preset);
	request.send();
    }
}

function select_device( device ){
    let request = new XMLHttpRequest();
    request.onload = function() {
	location.reload();
    }
    request.open('GET', 'rest.php?devselect=' + device);
    request.send();
}

function get_value_by_id( id, def='' ){
    var x = document.getElementById(id);
    if( x )
	return x.value;
    return def;
}

function apply_jack_settings(){
    var x = document.getElementById('jackvaluechanged');
    x.style.border = '';
    let request = new XMLHttpRequest();
    request.open('POST', '/rest.php', true);
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    var data = 'jackaudio=&jackplugdev=' + document.getElementById('jackplugdev').checked +
	'&jackdevice='+encodeURIComponent(get_value_by_id('jackdevice')) +
	'&jackrate='+get_value_by_id('jackrate') +
	'&jackperiod='+get_value_by_id('jackperiod') +
	'&jackbuffers='+get_value_by_id('jackbuffers');    
    request.send(data);
}

function switch_to_frontend( js ){
    if( js && (js.length>0)){
	frontend = JSON.parse(js);
	let request = new XMLHttpRequest();
	request.onload = function() {
	    location.href = frontend.ui;
	}
	request.open('POST', '/rest.php', true);
	request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	request.send('jsfrontendconfig='+js);
    }
}

function update_wifi(){
    var inp_wificb = document.getElementById('wifi');
    var inp_wifissid = document.getElementById('wifissid');
    var inp_wifipasswd = document.getElementById('wifipasswd');
    if( inp_wificb && inp_wifissid && inp_wifipasswd ){
	let request = new XMLHttpRequest();
	request.open('POST', '/rest.php', true);
	request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	var data = 'wifi='+inp_wificb.checked+'&wifissid='+inp_wifissid.value+'&wifipasswd='+inp_wifipasswd.value;
	request.send(data);
    }
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * End:
 */
