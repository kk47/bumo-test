+function($){
    // 参数选项设置...
    // 向jQuery原型中添加你的插件代码，用“pluginName”作为插件的函数名称。
    $.fn.setCustomValidity = function(msg) {
        // 遍历匹配的元素||元素集合，并返回this，以便进行链式调用。
        return this.each(function() {
            // 此处可通过this来获得每个单独的元素(jQuery对象)
            this.setCustomValidity(msg);
        });
    };
}(jQuery);

var ID = function(id) {
  return document.getElementById(id);
}
function convert_bytes(num, base, unit) {
    base = base || 1024;
    unit = unit || 'B';
    num = parseFloat(num) || 0;
    var log = 0;
    while(num > base){
      log++;
      num/=base;
    }
    var unit = {0: '', 1: 'K', 2: 'M', 3: 'G', 4: 'T', 5: 'P', 6: 'E', 6: 'Z'}[log] + unit;
    return parseFloat(num.toFixed(2)) + ' ' + unit;
}

function fixaxis(){
  var maxElement = this.axis.max;
  var kb = 1000,
    mb = Math.pow(1000, 2),
    gb = Math.pow(1000, 3);
  if (this.value == 0)
    return this.value;
  if (maxElement > gb) {
      return parseFloat((this.value / gb).toFixed(2)) + " GB/s";
  } else if (maxElement > mb) {
      return parseFloat((this.value / mb).toFixed(2)) + " MB/s";
  } else if (maxElement > kb) {
      return parseFloat((this.value / kb).toFixed(2)) + " KB/s";
  } else {
      return (this.value) + " B/s";
  }
}

function myconfirm(msg, call) {
    $("#cancel").hide();
    if (call) {
        $("#yes").show();
        if (msg.indexOf(':') == -1) {
            $("#cancel").show();
        }
        $("#yes").off().on("click", call);
    } else {
        $("#yes").off().on("click", function() {
            $("#confirmModal").modal("hide");
            $('button[name="upload"]').removeAttr("disabled");
        });
    }
    $("#confirmModal").modal("show");
    $("#alertmsg").text(msg);
}

function check_exists(target, type, noroot) {
  var path = $(target).val(),
    infoGlobal,
    url = "/checkExists?dest=" + path;

  if (type)
    url += "&type=" + type;

  if (noroot)
    url += "&noroot=true";

  $(target).setCustomValidity("");
  $.ajax({
    url: url,
    type: "GET",
    dataType: "JSON",
    async: false,
    success: function(info) {
      if (!info.res)
        $(target).setCustomValidity(info.msg);

      infoGlobal = info;
    }
  });
  return infoGlobal;
}

var submit = {
  selects: [],
  focusadd: {},
  url: '',
  async: true,
  refresh: function() {
    for (x in this.selects) {
      $(this.selects[x]).bootstrapTable("refresh");
    }

    if (this.focusadd && this.focusadd.value) {
      $(submit.selects[0]).one("load-success.bs.table", function() {
        var no = $(submit.selects[0]).bootstrapTable("checkBy", {field: submit.focusadd.key, values:[submit.focusadd.value]}).bootstrapTable("getSelections")[0].no;
        var ps = $(submit.selects[0]).bootstrapTable("getOptions").pageSize;
        $(submit.selects[0]).bootstrapTable("selectPage",Math.ceil(no/ps));
      });
    }
  },
  beforeSend: function(modal) {
    $(modal).modal('hide');
    $('#ProcessModal').modal('show');
    $("#waitting").show();
  },
  success: function(info) {
    $('#ProcessModal').one('hidden.bs.modal', function() {
      $("#waitting").hide();
      $("#success").hide();
      $("#failure").hide();
    });
    if (info.res) {
      $("#waitting").hide();
      $("#success").show();
      window.setTimeout(function() {
        $('#ProcessModal').modal('hide');
        submit.refresh();
      }, 1000);
    } else {
      $("#waitting").hide();
      if (info.msg)
        $("#failmsg").text(info.msg);
      $("#failure").show();
    }
  },
  get: function(data) {
    if (data.sync)
      this.async = false;
    this.selects = data.selects;
    return $.ajax({
      type: "GET",
      dataType: "json",
      async: this.async,
      url: data.url,
      beforeSend: this.beforeSend(data.modal),
      success: this.success,
    });
  },
  post: function(data) {
    if (data.sync)
      this.async = false;

    this.selects = data.selects;
    this.focusadd = data.focusadd;
    return $.ajax({
      type: "POST",
      dataType: "json",
      async: this.async,
      url: data.url,
      data: data.post,
      beforeSend: this.beforeSend(data.modal),
      success: this.success,
    });
  },
  postfile: function(data) {
    if (data.sync)
      this.async = false;

    this.selects = data.selects;
    this.focusadd = data.focusadd;
    return $.ajax({
      type: "POST",
      dataType: "json",
      async: this.async,
      url: data.url,
      data: data.post,
      beforeSend: this.beforeSend(data.modal),
      success: this.success,
      processData: false,  // 告诉jQuery不要去处理发送的数据
      contentType: false   // 告诉jQuery不要去设置Content-Type请求头
    });
  }
};

function show_btn_ctl(btn) {
  $(btn).show();
}

function fit(height, item, offset) {
  offset = offset || 140;
  item = item || 33;
  return parseInt((height - offset)/item);
}

function get_cookie(cookie_name) {
  var all_cookie = document.cookie,
    idx = all_cookie.indexOf(cookie_name);

  if (idx == -1) return '';

  return unescape(all_cookie.substring(idx + cookie_name.length + 1).split(';')[0]);
}

//Compatibility fixed
$('input[readonly]').focus(function() {
    this.blur();
});

$.ajaxSetup({ cache: false });

if (!String.prototype.repeat) {
  String.prototype.repeat = function(count) {
    'use strict';
    if (this == null) {
      throw new TypeError('can\'t convert ' + this + ' to object');
    }
    var str = '' + this;
    count = +count;
    if (count != count) {
      count = 0;
    }
    if (count < 0) {
      throw new RangeError('repeat count must be non-negative');
    }
    if (count == Infinity) {
      throw new RangeError('repeat count must be less than infinity');
    }
    count = Math.floor(count);
    if (str.length == 0 || count == 0) {
      return '';
    }
    // Ensuring count is a 31-bit integer allows us to heavily optimize the
    // main part. But anyway, most current (August 2014) browsers can't handle
    // strings 1 << 28 chars or longer, so:
    if (str.length * count >= 1 << 28) {
      throw new RangeError('repeat count must not overflow maximum string size');
    }
    var rpt = '';
    for (;;) {
      if ((count & 1) == 1) {
        rpt += str;
      }
      count >>>= 1;
      if (count == 0) {
        break;
      }
      str += str;
    }
    // Could we try:
    // return Array(count + 1).join(this);
    return rpt;
  }
}

if (!Array.prototype.find) {
  Object.defineProperty(Array.prototype, 'find', {
    value: function(predicate) {
     'use strict';
     if (this == null) {
       throw new TypeError('Array.prototype.find called on null or undefined');
     }
     if (typeof predicate !== 'function') {
       throw new TypeError('predicate must be a function');
     }
     var list = Object(this);
     var length = list.length >>> 0;
     var thisArg = arguments[1];
     var value;

     for (var i = 0; i < length; i++) {
       value = list[i];
       if (predicate.call(thisArg, value, i, list)) {
         return value;
       }
     }
     return undefined;
    }
  });
}

const reduce = Function.bind.call(Function.call, Array.prototype.reduce);
const isEnumerable = Function.bind.call(Function.call, Object.prototype.propertyIsEnumerable);
const concat = Function.bind.call(Function.call, Array.prototype.concat);
const keys = Reflect.ownKeys;

if (!Object.values) {
	Object.values = function values(O) {
		return reduce(keys(O), (v, k) => concat(v, typeof k === 'string' && isEnumerable(O, k) ? [O[k]] : []), []);
	};
}

if (!Object.entries) {
	Object.entries = function entries(O) {
		return reduce(keys(O), (e, k) => concat(e, typeof k === 'string' && isEnumerable(O, k) ? [[k, O[k]]] : []), []);
	};
}

var base64EncodeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
function base64encode(str){
    var out, i, len;
    var c1, c2, c3;
    len = str.length;
    i = 0;
    out = "";
    while (i < len) {
        c1 = str.charCodeAt(i++) & 0xff;
        if (i == len) {
            out += base64EncodeChars.charAt(c1 >> 2);
            out += base64EncodeChars.charAt((c1 & 0x3) << 4);
            out += "==";
            break;
        }
        c2 = str.charCodeAt(i++);
        if (i == len) {
            out += base64EncodeChars.charAt(c1 >> 2);
            out += base64EncodeChars.charAt(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4));
            out += base64EncodeChars.charAt((c2 & 0xF) << 2);
            out += "=";
            break;
        }
        c3 = str.charCodeAt(i++);
        out += base64EncodeChars.charAt(c1 >> 2);
        out += base64EncodeChars.charAt(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4));
        out += base64EncodeChars.charAt(((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6));
        out += base64EncodeChars.charAt(c3 & 0x3F);
    }
    return out;
}