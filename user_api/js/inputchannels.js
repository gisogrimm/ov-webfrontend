let inchannels = JSON.parse(jsinputchannels.value);
let hwinputs = JSON.parse(hwinputchannels.value);
let jsdevcfg = JSON.parse(devcfg.value);
const lo_cut = {
  'filter': {
    'mode': 'highpass',
    'fc': 120
  }
}
const lo_cut80 = {
  'filter': {
    'mode': 'highpass',
    'fc': 80
  }
}
const lo_cut40 = {
  'filter': {
    'mode': 'highpass',
    'fc': 40
  }
}
const eq_300 = {
  'filter': {
    'mode': 'equalizer',
    'fc': 300,
    'Q': 1,
    'gain': 3
  }
}
const eq_6000 = {
  'filter': {
    'mode': 'equalizer',
    'fc': 6000,
    'Q': 1,
    'gain': -3
  }
}
const tube_green = {
  'filter': {
    'mode': 'highpass',
    'fc': 80
  },
  'tubesim': {
    'bypass': false,
    'pregain': 9.1,
    'postgain': -13.2,
    'saturation': -40.0,
    'offset': 0,
    'wet': 1
  }
}
const tube_red = {
  'filter': {
    'mode': 'highpass',
    'fc': 140
  },
  'tubesim': {
    'bypass': false,
    'pregain': 40,
    'postgain': -17,
    'saturation': -40.0,
    'offset': 0.083,
    'wet': 1
  }
}
const spksim1 = {
  'spksim': {
    'bypass': false,
    'fres': 1200,
    'gain': 0,
    'q': 0.8,
    'scale': 0.5,
    'wet': 1
  }
}
const synth1 = {
    'simplesynth':{
        'autoconnect': 'true',
        'level': 73
    }
}
const synth2 = {
    'simplesynth': {
        'autoconnect': 'true',
        'decay': 20,
        'decaydamping': 0.07,
        'decayoffset': 0.2,
        'detune': -6.25,
        'f0': 440,
        'level': 79.2969,
        'onset': 0.04
    }
}
const synth3 = {
    'simplesynth': {
        'autoconnect': 'true',
        'decay': 40,
        'decaydamping': 0.07,
        'decayoffset': 0.2,
        'detune': -0.75,
        'f0': 415,
        'level': 79.2969,
        'onset': 0.14,
        'noiseweight': 0.1,
        'decaynoise': 0.1,
        'noiseq': 0.3,
        'gamma': 0.2,
        'partialweights':'1 1 0.316 1 0.282 1 2 0.0891 0.0398 0.0398 0.398',
        'tuning':'meantone4',
        'noisemin':0.01
    }
}

function inputchannels_add() {
  inchannels.push({
    sourceport: 'system:capture_1',
    position: {
      x: 0,
      y: 0,
      z: 0
    },
    gain: 1
  });
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_remove(rk) {
  var inchannels_ = [];
  var k;
  for (k = 0; k < inchannels.length; k++) {
    if (k != rk) inchannels_.push(inchannels[k]);
  }
  inchannels = inchannels_;
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_onedit_plugins(rk, value) {
  if (value.length > 0) {
    inchannels[rk]['plugins'] = JSON.parse(value);
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post('jsinputchannels', jsinputchannels.value);
  }
}

function inputchannels_onedit_port(rk, value) {
  if (value.length > 0) {
    inchannels[rk]['sourceport'] = value;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post('jsinputchannels', jsinputchannels.value);
  }
}

function inputchannels_onedit_directivity(rk, value) {
  if (value.length > 0) {
    inchannels[rk]['directivity'] = value;
    jsinputchannels.value = JSON.stringify(inchannels);
    rest_setval_post('jsinputchannels', jsinputchannels.value);
  }
}

function inputchannels_onedit_x(rk, value) {
  inchannels[rk]['position']['x'] = value;
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_onedit_y(rk, value) {
  inchannels[rk]['position']['y'] = value;
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_onedit_z(rk, value) {
  inchannels[rk]['position']['z'] = value;
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_onedit_gain(rk, value) {
  inchannels[rk]['gain'] = Math.pow(10, 0.05 * Number.parseFloat(value));
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_onedit_name(rk, value) {
  inchannels[rk]['name'] = value;
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_preset(p) {
  if (p == "p0") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: -0.08,
        y: 0,
        z: -0.03
      },
      gain: 1,
      directivity: 'cardioid'
    });
  }
  if (p == "p1") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: 0.3,
        y: 0,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "p2") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_2',
      position: {
        x: 0.3,
        y: 0,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "p12dual") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: 0.3,
        y: -0.7,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni'
    });
    inchannels.push({
      sourceport: 'system:capture_2',
      position: {
        x: 0.3,
        y: 0.7,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "p12dualviolin") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: 0.3,
        y: -0.7,
        z: 0
      },
      gain: 1,
      directivity: 'omni'
    });
    inchannels.push({
      sourceport: 'system:capture_2',
      position: {
        x: 0.3,
        y: 0.7,
        z: 0
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "p1violin") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: 0.3,
        y: 0,
        z: 0
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "p12dualvoc") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_1',
      position: {
        x: 0.08,
        y: 0,
        z: -0.07
      },
      gain: 1,
      directivity: 'omni',
      name: 'voc'
    });
    inchannels.push({
      sourceport: 'system:capture_2',
      position: {
        x: 0.3,
        y: -0.1,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni',
      name: 'guit'
    });
  }
  if (p == "p12single") {
    inchannels = [];
    inchannels.push({
      sourceport: 'system:capture_[12]',
      position: {
        x: 0.3,
        y: 0,
        z: -0.6
      },
      gain: 1,
      directivity: 'omni'
    });
  }
  if (p == "listen") {
    inchannels = [];
  }
  jsinputchannels.value = JSON.stringify(inchannels);
  rest_setval_post('jsinputchannels', jsinputchannels.value);
}

function inputchannels_createUI() {
  while (jsinputchannelsdiv.firstChild) {
    jsinputchannelsdiv.removeChild(jsinputchannelsdiv.lastChild);
  }
  // show label:
  var el = document.createElement('label');
  el.appendChild(document.createTextNode(
      translate('configure input channels (to which your microphones/instruments are connected):')
    ));
  jsinputchannelsdiv.appendChild(el);
  jsinputchannelsdiv.appendChild(document.createElement('br'));
  // preset selector:
  var el = document.createElement('select');
  el.setAttribute('onchange',
    '{inputchannels_preset(this.value);inputchannels_createUI()}');
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'none');
  opt.setAttribute('selected', '');
    opt.appendChild(document.createTextNode('-- '+translate('select channel preset')+' --'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p0');
  opt.appendChild(document.createTextNode('send first input, vocals'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p1');
  opt.appendChild(document.createTextNode('send first input, gamba/guitar'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p1violin');
  opt.appendChild(document.createTextNode('send first input, violin/trumpet'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p12dual');
  opt.appendChild(document.createTextNode('send both inputs, gamba/guitar'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p12dualviolin');
  opt.appendChild(document.createTextNode('send both inputs, violin/trumpet'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p12dualvoc');
  opt.appendChild(document.createTextNode('send both inputs, vocals + guitar'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'p12single');
  opt.appendChild(document.createTextNode(
    'send both inputs downmixed, gamba/guitar'));
  var opt = el.appendChild(document.createElement('option'));
  opt.setAttribute('value', 'listen');
  opt.appendChild(document.createTextNode('listening only'));
  jsinputchannelsdiv.appendChild(el);
  //// advanced channel configuration:
  jsinputchannelsdiv.appendChild(document.createElement('br'));
  // div for advanced channel configuration:
  var adiv = document.createElement('div');
  adiv.setAttribute('id', 'advancedchannelsettings');
  adiv.setAttribute('style', 'display: block;');
  adiv.setAttribute('class', 'devprop');
  jsinputchannelsdiv.appendChild(adiv);
  var el = document.createElement('input');
    el.setAttribute('value', translate('add channel'));
  el.setAttribute('type', 'button');
  el.setAttribute('onclick', '{inputchannels_add();inputchannels_createUI()}');
  adiv.appendChild(el);
  var el = document.createElement('label');
  el.appendChild(document.createTextNode(
      ' ('+translate('positions are relative to the center of your head, in meters')+')'));
  adiv.appendChild(el);
  adiv.appendChild(document.createElement('br'));
  if (inchannels) {
    for (var k = 0; k < inchannels.length; k++) {
      var cdiv = adiv.appendChild(document.createElement('dev'));
      cdiv.setAttribute('class', 'channelcfg');
      var box0 = cdiv.appendChild(document.createElement('div'));
      box0.setAttribute('class', 'plugincategory');
      // boxes with labels
      var box1 = box0.appendChild(document.createElement('div'));
      box1.setAttribute('class', 'toplabelbox');
      var tlab1 = box1.appendChild(document.createElement('div'));
      tlab1.setAttribute('class', 'toplabel');
        tlab1.appendChild(document.createTextNode(translate('source port')+':'));
      var el = document.createElement('select');
      el.setAttribute('onchange', '{inputchannels_onedit_port(' + k.toString(
        10) + ',this.value);inputchannels_createUI();}');
      var eopt = el.appendChild(document.createElement('option'));
      eopt.setAttribute('value', '');
      eopt.appendChild(document.createTextNode('- select channel -'));
      el.appendChild(eopt);

      function add_opt(optv, ind, options) {
        var opt = el.appendChild(document.createElement('option'));
        opt.setAttribute('value', optv);
        opt.appendChild(document.createTextNode(optv));
        if (inchannels[k]['sourceport'] == optv) opt.setAttribute('selected',
          '');
        el.appendChild(opt);
      }
      if (Array.isArray(hwinputs)) hwinputs.forEach(add_opt);
      box1.appendChild(el);
      var box2 = box0.appendChild(document.createElement('div'));
      box2.setAttribute('class', 'toplabelbox');
      var tlab2 = box2.appendChild(document.createElement('div'));
      tlab2.setAttribute('class', 'toplabel');
        tlab2.appendChild(document.createTextNode(translate('position')+':'));
      var el = document.createElement('input');
      el.setAttribute('value', (inchannels[k]['position']['x']).toFixed(2));
      el.setAttribute('onchange', '{inputchannels_onedit_x(' + k.toString(10) +
        ',this.value);}');
      el.setAttribute('size', '3');
      el.setAttribute('title',
        'x position (positive values are in front of you)');
      box2.appendChild(el);
      var el = document.createElement('input');
      el.setAttribute('value', (inchannels[k]['position']['y']).toFixed(2));
      el.setAttribute('onchange', '{inputchannels_onedit_y(' + k.toString(10) +
        ',this.value);}');
      el.setAttribute('size', '3');
      el.setAttribute('title', 'y position (positive values are to your left)');
      box2.appendChild(el);
      var el = document.createElement('input');
      el.setAttribute('value', (inchannels[k]['position']['z']).toFixed(2));
      el.setAttribute('onchange', '{inputchannels_onedit_z(' + k.toString(10) +
        ',this.value);}');
      el.setAttribute('size', '3');
      el.setAttribute('title',
        'z position (positive values are above your ears)');
      box2.appendChild(el);
      // source directivity:
      var box3 = box0.appendChild(document.createElement('div'));
      box3.setAttribute('class', 'toplabelbox');
      var tlab3 = box3.appendChild(document.createElement('div'));
      tlab3.setAttribute('class', 'toplabel');
        tlab3.appendChild(document.createTextNode(translate('directivity')+':'));
      var el = document.createElement('select');
      el.setAttribute('onchange', '{inputchannels_onedit_directivity(' + k
        .toString(10) + ',this.value);inputchannels_createUI();}');

      function add_opt_dir(optv, ind, options) {
        var opt = el.appendChild(document.createElement('option'));
        opt.setAttribute('value', optv);
        opt.appendChild(document.createTextNode(optv));
        if (inchannels[k]['directivity'] == optv) opt.setAttribute('selected',
          '');
        el.appendChild(opt);
      }
      ['omni', 'cardioid'].forEach(add_opt_dir);
      box3.appendChild(el);
      // end source directivity.
      // name:
      var box4 = box0.appendChild(document.createElement('div'));
      box4.setAttribute('class', 'toplabelbox');
      var tlab4 = box4.appendChild(document.createElement('div'));
      tlab4.setAttribute('class', 'toplabel');
        tlab4.appendChild(document.createTextNode(translate('name')+':'));
      var el = document.createElement('input');
      if (inchannels[k]['name']) el.setAttribute('value', inchannels[k][
      'name']);
      else el.setAttribute('value', '');
      el.setAttribute('onchange', '{inputchannels_onedit_name(' + k.toString(
        10) + ',this.value);}');
      el.setAttribute('size', '10');
      el.setAttribute('title', 'channel name (optional)');
      box4.appendChild(el);
      // remove:
      var el = document.createElement('input');
      el.setAttribute('value', 'remove channel');
      el.setAttribute('type', 'button');
      el.setAttribute('onclick', '{inputchannels_remove(' + k.toString(10) +
        ');inputchannels_createUI()}');
      cdiv.appendChild(el);
      cdiv.appendChild(document.createElement('br'));
      // gain:
      var box4a = cdiv.appendChild(document.createElement('div'));
      box4a.setAttribute('class', 'plugincategory');
      var tlab4a = box4a.appendChild(document.createElement('div'));
      tlab4a.setAttribute('class', 'toplabel');
        tlab4a.appendChild(document.createTextNode(translate('Gain')+':'));
      el = box4a.appendChild(document.createElement('input'));
      el.setAttribute('onchange', '{inputchannels_onedit_gain(' + k.toString(
        10) + ',this.value);}');
      el.setAttribute('title', 'channel gain in dB');
      el.setAttribute('type', 'number');
      el.setAttribute('min', '-40');
      el.setAttribute('max', '20');
      el.setAttribute('step', '0.1');
      el.setAttribute('value', (20.0 * Math.log10(Math.max(0.01, Number
        .parseFloat(inchannels[k].gain)))).toFixed(1));
      cdiv.appendChild(document.createElement('br'));
      // plugins:
      var box5 = cdiv.appendChild(document.createElement('div'));
      box5.setAttribute('class', 'plugincategory');
      var tlab5 = box5.appendChild(document.createElement('div'));
      tlab5.setAttribute('class', 'plugincategorylab');
      tlab5.appendChild(document.createTextNode('Plugin presets:'));
      if (jsdevcfg.canplugins) {
        if (inchannels[k]['plugins'] == null) inchannels[k]['plugins'] = {};
        var el = box5.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'direct');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) + ',"{}");inputchannels_createUI();}');
        var odrv = box5.appendChild(document.createElement('div'));
        odrv.setAttribute('class', 'plugincategory');
        var odrvlab = odrv.appendChild(document.createElement('div'));
        odrvlab.setAttribute('class', 'plugincategorylab');
          odrvlab.appendChild(document.createTextNode(translate('overdrives')));
        var el = odrv.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'tube1');
        el.setAttribute('style', 'background-color: #239617;');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(tube_green));inputchannels_createUI();}');
        var el = odrv.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'tube2');
        el.setAttribute('style', 'background-color: #c81c1d;');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(tube_red));inputchannels_createUI();}');
        var el = odrv.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'speaker');
        //el.setAttribute('style','background-color: #c81c1d;');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(spksim1));inputchannels_createUI();}');
        var flts = box5.appendChild(document.createElement('div'));
        flts.setAttribute('class', 'plugincategory');
        var fltslab = flts.appendChild(document.createElement('div'));
        fltslab.setAttribute('class', 'plugincategorylab');
          fltslab.appendChild(document.createTextNode(translate('low cuts')));
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', '120 Hz');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(lo_cut));inputchannels_createUI();}');
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', '80 Hz');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(lo_cut80));inputchannels_createUI();}');
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', '40 Hz');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(lo_cut40));inputchannels_createUI();}');
        // equalizers:
        var flts = box5.appendChild(document.createElement('div'));
        flts.setAttribute('class', 'plugincategory');
        var fltslab = flts.appendChild(document.createElement('div'));
        fltslab.setAttribute('class', 'plugincategorylab');
          fltslab.appendChild(document.createTextNode(translate('equalizers')));
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'EQ 300 +3 (warm)');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(eq_300));inputchannels_createUI();}');
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'EQ 6kHz -3 (hiss)');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(eq_6000));inputchannels_createUI();}');
        // synths:
        var flts = box5.appendChild(document.createElement('div'));
        flts.setAttribute('class', 'plugincategory');
        var fltslab = flts.appendChild(document.createElement('div'));
        fltslab.setAttribute('class', 'plugincategorylab');
          fltslab.appendChild(document.createTextNode(translate('synths')));
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'synth1');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(synth1));inputchannels_createUI();}');
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'synth2');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(synth2));inputchannels_createUI();}');
        var el = flts.appendChild(document.createElement('input'));
        el.setAttribute('type', 'button');
        el.setAttribute('value', 'synth3');
        el.setAttribute('onclick', '{inputchannels_onedit_plugins(' + k
          .toString(10) +
          ',JSON.stringify(synth3));inputchannels_createUI();}');
        // plugin name list:
        var divpn = box5.appendChild(document.createElement('div'));
        divpn.setAttribute('class', 'pluginnamelist');

        function add_plugname(optv) {
          var opt = divpn.appendChild(document.createElement('div'));
          opt.setAttribute('class', 'pluginname');
          opt.appendChild(document.createTextNode(optv));
        }
        for (var plug in inchannels[k]['plugins'])
          if (Object.prototype.hasOwnProperty.call(inchannels[k]['plugins'],
              plug)) add_plugname(plug);
        // expert settings:
        var ediv = cdiv.appendChild(document.createElement('div'));
        ediv.setAttribute('class', 'showexpertsettings');
        if (!jsdevcfg.showexpertsettings) ediv.setAttribute('style',
          'display: none;');
        var el = ediv.appendChild(document.createElement('label'));
          el.appendChild(document.createTextNode(translate('source port')+': '));
        var el = ediv.appendChild(document.createElement('input'));
        el.setAttribute('value', inchannels[k]['sourceport']);
        el.setAttribute('onchange', '{inputchannels_onedit_port(' + k.toString(
          10) + ',this.value);inputchannels_createUI();}');
        el.setAttribute('title', 'source port name');
        var el = ediv.appendChild(document.createElement('br'));
        var el = ediv.appendChild(document.createElement('textarea'));
        //el.setAttribute('type','edit');
        el.setAttribute('rows', '6');
        el.appendChild(document.createTextNode(JSON.stringify(inchannels[k][
          'plugins'
        ], null, 2)));
        el.setAttribute('onchange', '{inputchannels_onedit_plugins(' + k
          .toString(10) + ',this.value);inputchannels_createUI();}');
        el.setAttribute('style', 'width: 98%;');
      }
    }
  }
}
inputchannels_createUI();
// Local Variables:
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// compile-command: "js-beautify -d -s 2 -w 80 -r inputchannels.js"
// End:
