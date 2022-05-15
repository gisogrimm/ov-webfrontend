let inchannels = JSON.parse(jsinputchannels.value);
let hwinputs = JSON.parse(hwinputchannels.value);
let jsdevcfg = JSON.parse(devcfg.value);

const tube_green = {
    'filter' : {
        'mode':'highpass',
        'fc':80
    },
    'tubesim' : {
        'bypass':false,
        'pregain':9.1,
        'postgain':-13.2,
        'saturation':-40.0,
        'offset':0,
        'wet':1
    }
}

const tube_red = {
    'filter' : {
        'mode':'highpass',
        'fc':140
    },
    'tubesim' : {
        'bypass':false,
        'pregain':40,
        'postgain':-17,
        'saturation':-40.0,
        'offset':0.083,
        'wet':1
    }
}

const spksim1 = {
    'spksim' : {
        'bypass':false,
        'fres':1200,
        'gain':0,
        'q':0.8,
        'scale':0.5,
        'wet':1
    }
}

function inputchannels_add() {
    inchannels.push({sourceport:'system:capture_1',position:{x:0,y:0,z:0},gain:1});
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_remove( rk ) {
    var inchannels_ = [];
    var k;
    for( k = 0; k < inchannels.length; k++ ){
	if( k != rk )
	    inchannels_.push(inchannels[k]);
    }
    inchannels = inchannels_;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_onedit_plugins( rk, value ) {
    if( value.length > 0 ){
	inchannels[rk]['plugins'] = JSON.parse(value);
	jsinputchannels.value = JSON.stringify(inchannels);
	rest_setval_post( 'jsinputchannels', jsinputchannels.value );
    }
}

function inputchannels_onedit_port( rk, value ) {
    if( value.length > 0 ){
	inchannels[rk]['sourceport'] = value;
	jsinputchannels.value = JSON.stringify(inchannels);
	rest_setval_post( 'jsinputchannels', jsinputchannels.value );
    }
}

function inputchannels_onedit_directivity( rk, value ) {
    if( value.length > 0 ){
	inchannels[rk]['directivity'] = value;
	jsinputchannels.value = JSON.stringify(inchannels);
	rest_setval_post( 'jsinputchannels', jsinputchannels.value );
    }
}

function inputchannels_onedit_x( rk, value ) {
    inchannels[rk]['position']['x'] = value;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_onedit_y( rk, value ) {
    inchannels[rk]['position']['y'] = value;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_onedit_z( rk, value ) {
    inchannels[rk]['position']['z'] = value;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_preset( p ){
    if(p=="p0"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.08,y:0,z:-0.07},gain:1,directivity:'cardioid'});
    }
    if(p=="p1"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.3,y:0,z:-0.6},gain:1,directivity:'omni'});
    }
    if(p=="p2"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_2',position:{x:0.3,y:0,z:-0.6},gain:1,directivity:'omni'});
    }
    if(p=="p12dual"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.3,y:-0.7,z:-0.6},gain:1,directivity:'omni'});
	inchannels.push({sourceport:'system:capture_2',position:{x:0.3,y:0.7,z:-0.6},gain:1,directivity:'omni'});
    }
    if(p=="p12dualviolin"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.3,y:-0.7,z:0},gain:1,directivity:'omni'});
	inchannels.push({sourceport:'system:capture_2',position:{x:0.3,y:0.7,z:0},gain:1,directivity:'omni'});
    }
    if(p=="p1violin"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.3,y:0,z:0},gain:1,directivity:'omni'});
    }
    if(p=="p12dualvoc"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_1',position:{x:0.08,y:0,z:-0.07},gain:1,directivity:'omni'});
	inchannels.push({sourceport:'system:capture_2',position:{x:0.3,y:-0.1,z:-0.6},gain:1,directivity:'omni'});
    }
    if(p=="p12single"){
	inchannels = [];
	inchannels.push({sourceport:'system:capture_[12]',position:{x:0.3,y:0,z:-0.6},gain:1,directivity:'omni'});
    }
    if(p=="listen"){
	inchannels = [];
    }
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post( 'jsinputchannels', jsinputchannels.value );
}

function inputchannels_createUI( ) {
    while (jsinputchannelsdiv.firstChild) {
	jsinputchannelsdiv.removeChild(jsinputchannelsdiv.lastChild);
    }
    // show label:
    var el = document.createElement('label');
    el.appendChild(document.createTextNode('configure input channels (to which your microphones/instruments are connected): '));
    jsinputchannelsdiv.appendChild(el);
    jsinputchannelsdiv.appendChild(document.createElement('br'));
    // preset selector:
    var el = document.createElement('select');
    //el.setAttribute('oninput','rest_setval_post( 'jsinputchannels', jsinputchannels.value );');
    el.setAttribute('onchange','{inputchannels_preset(this.value);inputchannels_createUI()}');
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','none');
    opt.setAttribute('selected','');
    opt.appendChild(document.createTextNode('-- select channel preset --'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p0');
    opt.appendChild(document.createTextNode('send first input, vocals'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p1');
    opt.appendChild(document.createTextNode('send first input, gamba/guitar'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p1violin');
    opt.appendChild(document.createTextNode('send first input, violin/trumpet'));
    //var opt = el.appendChild(document.createElement('option'));
    //opt.setAttribute('value','p2');
    //opt.appendChild(document.createTextNode('send second input, gamba/guitar'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p12dual');
    opt.appendChild(document.createTextNode('send both inputs, gamba/guitar'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p12dualviolin');
    opt.appendChild(document.createTextNode('send both inputs, violin/trumpet'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p12dualvoc');
    opt.appendChild(document.createTextNode('send both inputs, vocals + guitar'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','p12single');
    opt.appendChild(document.createTextNode('send both inputs downmixed, gamba/guitar'));
    var opt = el.appendChild(document.createElement('option'));
    opt.setAttribute('value','listen');
    opt.appendChild(document.createTextNode('listening only'));
    jsinputchannelsdiv.appendChild(el);
    //// advanced channel configuration:
    //var inp = document.createElement('input');
    //inp.setAttribute('type','button');
    //inp.setAttribute('class','roomsettingstoggle uibutton');
    //inp.setAttribute('onclick','toggledisplay(\'advancedchannelsettings\',\'advanced channel configuration\');');
    //inp.setAttribute('value','show advanced channel configuration');
    //jsinputchannelsdiv.appendChild(inp);
    jsinputchannelsdiv.appendChild(document.createElement('br'));
    // div for advanced channel configuration:
    var adiv = document.createElement('div');
    adiv.setAttribute('id','advancedchannelsettings');
    adiv.setAttribute('style','display: block;');
    adiv.setAttribute('class','devprop');
    jsinputchannelsdiv.appendChild(adiv);
    var el = document.createElement('input');
    el.setAttribute('value','add channel');
    el.setAttribute('type','button');
    el.setAttribute('onclick','{inputchannels_add();inputchannels_createUI()}');
    adiv.appendChild(el);
    var el = document.createElement('label');
    el.appendChild(document.createTextNode(' (positions are relative to the center of your head, in meters)'));
    adiv.appendChild(el);
    adiv.appendChild(document.createElement('br'));
    if( inchannels ){
	for( var k = 0; k < inchannels.length; k++ ){
            var cdiv = adiv.appendChild(document.createElement('dev'));
            cdiv.setAttribute('class','channelcfg');
            var el = cdiv.appendChild(document.createElement('input'));
	    el.setAttribute('value',inchannels[k]['sourceport']);
	    el.setAttribute('onchange','{inputchannels_onedit_port('+k.toString(10)+',this.value);inputchannels_createUI();}');
	    el.setAttribute('title','source port name');
	    var el = document.createElement('select');
	    el.setAttribute('onchange','{inputchannels_onedit_port('+k.toString(10)+',this.value);inputchannels_createUI();}');
	    var eopt = el.appendChild(document.createElement('option'));
	    eopt.setAttribute('value','');
	    eopt.appendChild(document.createTextNode('- select channel -'));
	    el.appendChild(eopt);
	    function add_opt(optv,ind,options){
		var opt = el.appendChild(document.createElement('option'));
		opt.setAttribute('value',optv);
		opt.appendChild(document.createTextNode(optv));
		if( inchannels[k]['sourceport'] == optv )
		    opt.setAttribute('selected','');
		el.appendChild(opt);
	    }
	    if( Array.isArray(hwinputs)) 
		hwinputs.forEach(add_opt);
	    cdiv.appendChild(el);
	    var el = document.createElement('input');
	    el.setAttribute('value',inchannels[k]['position']['x']);
	    el.setAttribute('onchange','{inputchannels_onedit_x('+k.toString(10)+',this.value);}');
	    el.setAttribute('size','1');
	    el.setAttribute('title','x position (positive values are in front of you)');
	    cdiv.appendChild(el);
	    var el = document.createElement('input');
	    el.setAttribute('value',inchannels[k]['position']['y']);
	    el.setAttribute('onchange','{inputchannels_onedit_y('+k.toString(10)+',this.value);}');
	    el.setAttribute('size','1');
	    el.setAttribute('title','y position (positive values are to your left)');
	    cdiv.appendChild(el);
	    var el = document.createElement('input');
	    el.setAttribute('value',inchannels[k]['position']['z']);
	    el.setAttribute('onchange','{inputchannels_onedit_z('+k.toString(10)+',this.value);}');
	    el.setAttribute('size','1');
	    el.setAttribute('title','z position (positive values are above your ears)');
	    cdiv.appendChild(el);
	    // source directivity:
	    var el = document.createElement('select');
	    el.setAttribute('onchange','{inputchannels_onedit_directivity('+k.toString(10)+',this.value);inputchannels_createUI();}');
	    function add_opt_dir(optv,ind,options){
		var opt = el.appendChild(document.createElement('option'));
		opt.setAttribute('value',optv);
		opt.appendChild(document.createTextNode(optv));
		if( inchannels[k]['directivity'] == optv )
		    opt.setAttribute('selected','');
		el.appendChild(opt);
	    }
	    ['omni','cardioid'].forEach(add_opt_dir);
	    cdiv.appendChild(el);
	    // end source directivity.
	    var el = document.createElement('input');
	    el.setAttribute('value','remove channel');
	    el.setAttribute('type','button');
	    el.setAttribute('onclick','{inputchannels_remove('+k.toString(10)+');inputchannels_createUI()}');
	    cdiv.appendChild(el);
	    cdiv.appendChild(document.createElement('br'));
            if( jsdevcfg.canplugins ){
                if( inchannels[k]['plugins'] == null )
                    inchannels[k]['plugins'] = {};
	        cdiv.appendChild(document.createTextNode(' Plugin presets: '));
                var el = cdiv.appendChild(document.createElement('input'));
                el.setAttribute('type','button');
                el.setAttribute('value','direct');
	        el.setAttribute('onclick','{inputchannels_onedit_plugins('+k.toString(10)+',"{}");inputchannels_createUI();}');
                var odrv = cdiv.appendChild(document.createElement('div'));
                odrv.setAttribute('class','plugincategory');
                var odrvlab = odrv.appendChild(document.createElement('div'));
                odrvlab.setAttribute('class','plugincategorylab');
                odrvlab.appendChild(document.createTextNode('overdrives'));
                var el = odrv.appendChild(document.createElement('input'));
                el.setAttribute('type','button');
                el.setAttribute('value','tube1');
                el.setAttribute('style','background-color: #239617;');
	        el.setAttribute('onclick','{inputchannels_onedit_plugins('+k.toString(10)+',JSON.stringify(tube_green));inputchannels_createUI();}');
                var el = odrv.appendChild(document.createElement('input'));
                el.setAttribute('type','button');
                el.setAttribute('value','tube2');
                el.setAttribute('style','background-color: #c81c1d;');
	        el.setAttribute('onclick','{inputchannels_onedit_plugins('+k.toString(10)+',JSON.stringify(tube_red));inputchannels_createUI();}');
                var el = odrv.appendChild(document.createElement('input'));
                el.setAttribute('type','button');
                el.setAttribute('value','speaker');
                //el.setAttribute('style','background-color: #c81c1d;');
	        el.setAttribute('onclick','{inputchannels_onedit_plugins('+k.toString(10)+',JSON.stringify(spksim1));inputchannels_createUI();}');
                var ediv = cdiv.appendChild(document.createElement('div'));
                ediv.setAttribute('class','showexpertsettings');
                if( !jsdevcfg.showexpertsettings )
                    ediv.setAttribute('style','display: none;');
	        var el = ediv.appendChild(document.createElement('textarea'));
                //el.setAttribute('type','edit');
                el.setAttribute('rows','6');
	        el.appendChild(document.createTextNode(JSON.stringify(inchannels[k]['plugins'],null,2)));
	        //el.setAttribute('title','plugin configuration');
	        el.setAttribute('onchange','{inputchannels_onedit_plugins('+k.toString(10)+',this.value);inputchannels_createUI();}');
                el.setAttribute('style','width: 98%;');
            }
        }
    }
}

inputchannels_createUI();
