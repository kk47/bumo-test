var pathJson = {
  path: "Lw==",
  hastrans: false,
  offset: undefined,
  "navlist": [{
    name: "Home",
    path: Dayudir || "Lw==",
    transcode: false
  }],
  bs: undefined,
  refresh: false
};

var level_path;

function setIcon(value) {
  if (value == Directory) {
    return '<span class="fa fa-folder-open-o fa-lg fa-sw" aria-hidden="true"></span>'
  } else if (value == File) {
    return '<span class="fa fa-file-o fa-lg fa-sw" aria-hidden="true"></span>'
  } else {
    return '<span class="fa fa-link fa-lg fa-sw" aria-hidden="true"></span>'
  }
}

function sizeConv(value) {
  if (value) {
    return convert_bytes(value);
  } else {
    return value;
  }
}

function setParams(params) {
  pathJson.offset = params.offset;
  params["refresh"] = JSON.stringify(pathJson.refresh);
  pathJson.refresh = false;
  return params;
}

function enter_directory(rowData, trObj) {
  if (rowData.type != Directory) {
    return false;
  }

  pathJson["path"] = rowData.b64Path;
  pathJson["navlist"].push({
    "name": rowData.name.name,
    "path": pathJson["path"],
    "transcode": rowData.name.transcode
  });
  var url = "/browser?path=" + rowData.b64Path;

  //bs.bootstrapTable("resetSearch");
  pathJson.bs.bootstrapTable("refresh", {
    url: url
  });
}

function fastJump(idx) {
  pathJson["navlist"] = pathJson["navlist"].slice(0, idx + 1);
  pathJson["path"] = pathJson["navlist"][idx].path;
  pathJson["hastrans"] = false;
  showCtl();
  var url = "/browser?path=" + pathJson["navlist"][idx].path;
  //bs.bootstrapTable("resetSearch");
  pathJson.bs.bootstrapTable("refresh", {
    url: url
  });
}

function pathNav(data) {
  var div = $("ol.breadcrumb");
  div.children().remove();

  var nav = pathJson["navlist"];
  var li = new String();
  nav.forEach(function(e, i, nav) {
    if (nav.length == 1) {
      li = '<li class="active">Home</li>';
      div.append(li);
    } else {
      var name = nav[i].name;
      var hastrans = nav[i].transcode
      if (hastrans) {
        pathJson["hastrans"] = true;
      }

      if (i == nav.length - 1) {
        li = '<li class="active the-browser-nav" title="' + name + '"';
        if (hastrans) {
          li += ' style="color: red;"';
        }
        li += '>' + name + '</li>';
        div.append(li);
      } else {
        li = "<li class='the-browser-nav' title='" + name + "'></li>";
        div.append(li);
        var a = new String();

        a = '<a onclick="fastJump(' + i + ')" href="javascript:void(0)"';
        if (hastrans) {
          a += ' style="color: red;"'
        }
        a += ">" + name + "</a>";
        li = div.children("li:last");
        li.append(a);
      }
    }
  });
}

function colorSet(value) {
  var html = new String();
  if (value.transcode) {
    html = '<span style="color: red;">' + value.name + "</span>"
  } else {
    html = "<span>" + value.name + "</span>"
  }
  return html;
}

function browse_init() {
  pathJson['path'] = Dayudir || "Lw==";
  pathJson["navlist"] = pathJson["navlist"].slice(0, 1);
  pathJson.refresh = true;
  var url = "/browser?path=" + pathJson['path'];
  pathJson.bs.bootstrapTable("refresh", {
    "url": url
  });
}

function showMsg(msg) {
  $('#browserModal div[name="contorl"]').toggleClass("hidden", true);
  $('#browserModal div[name="sign"]').toggleClass("hidden", false);
  $("#browserModal div.modal-footer strong").text(msg);
}

function showCtl() {
  $('#browserModal div[name="contorl"]').toggleClass("hidden", false);
  $('#browserModal div[name="sign"]').toggleClass("hidden", true);
}

$("#refresh").click(function() {
  pathJson.refresh = true;
  $(pathJson.bs).bootstrapTable('refresh');
})

var browser = {
  iniFlag: false,
  jsonObj: undefined,
  initial: function() {
    this.iniFlag = true;
    $("#browser_content").bootstrapTable({
      classes: "the-table",
      toolbar: "#refreshDiv",
      toolbarAlign: "right",
      striped: true,
      pagination: true,
      pageSize: 10,
      pageList: [10, 20, 40, 100],
      height: 435,
      search: true,
      clickToSelect: true,
      singleSelect: true,
      sortName: 'name',
      onLoadSuccess: pathNav,
      onDblClickRow: enter_directory,
      queryParams: setParams,
      columns: [{
        field: 'type',
        align: 'center',
        width: '5%',
        formatter: setIcon,
      }, {
        field: 'name',
        title: Name,
        halign: 'center',
        align: 'left',
        sortable: true,
        valign: 'middle',
        width: '58%',
        formatter: colorSet,
      }, {
        field: 'mtime',
        title: Mtime,
        align: 'center',
        valign: 'middle',
        sortable: true,
        width: '17%',
      }, {
        field: 'type',
        title: Type,
        align: 'center',
        valign: 'middle',
        sortable: true,
        width: '10%',
      }, {
        field: 'size',
        title: Size,
        halign: 'center',
        align: 'right',
        valign: 'middle',
        sortable: true,
        width: '10%',
        formatter: sizeConv,
      }, {
        checkbox: true,
        classes: "hide"
      }]
    });
    pathJson.bs = $('#browser_content');

    if (browser.jsonObj.homepage) {
      $("#browser_content").bootstrapTable('resetView');
      browse_init();
    }

    $('#browserModal button[name="confirm"]').on("click", function() {
      var arr = pathJson.bs.bootstrapTable("getSelections")[0];
      if (!arr)
        return;

      type = arr.type;

      if (arr.length == 0) {
        showMsg(typeErr);
        return false;
      }

      if (browser.jsonObj.selectType) {
        if (browser.jsonObj.selectType.indexOf(type) == -1) {
          if (JSON.stringify(browser.jsonObj.selectType) == JSON.stringify([Directory])) {
            showMsg(typeErr2);
          } else if (JSON.stringify(browser.jsonObj.selectType) == JSON.stringify([File])) {
            showMsg(typeErr3);
          } else {
            showMsg(typeErr);
          }
          return false;
        }
      }

      if (arr.name.transcode) {
        showMsg(pathErr);
        return false;
      }

      if (pathJson.hastrans) {
        showMsg(pathErr2);
        return false;
      }

      browser.jsonObj.type = type;

      if (browser.jsonObj.modalTarget)
        $(browser.jsonObj.modalTarget + " " + browser.jsonObj.inputTarget).val(arr.absPath).change();
      else
        $(browser.jsonObj.inputTarget).val(arr.absPath).change();

      if (browser.jsonObj.call) {
        browser.jsonObj.call();
      }

      $('#browserModal').modal("hide");
    });

    $('#browserModal').on('hidden.bs.modal', function() {
      if (browser.jsonObj.modalTarget)
        $(browser.jsonObj.modalTarget + " " + browser.jsonObj.inputTarget).focus();
      else
        $(browser.jsonObj.inputTarget).focus();
    });

    $('button[name="close-alert"]').on("click", function() {
      showCtl();
    });
  },
  modal: function(jsonObj) {
    this.jsonObj = jsonObj;
    if (!this.iniFlag)
      this.initial();
    $("#browserModal").modal("show");
    //$("#browser_content").bootstrapTable("refresh");
    return true;
  }
}

function enable_fattr() {
  $("#fileattr").removeAttr("disabled");
}

function disable_fattr() {
  $("#fileattr").attr("disabled", true);
}

function filesys() {
  height = $(".show-detail-browse").height();
  $("#filesys_content").bootstrapTable({
    toolbar: "#refreshDiv",
    toolbarAlign: "right",
    iconSize: "sm",
    striped: true,
    pagination: true,
    height: fit(height, 33, 189) * 33 + 148,
    pageSize: fit(height, 33, 189),
    pageList: [fit(height, 33, 189), fit(height, 33, 189) * 2],
    search: true,
    clickToSelect: true,
    singleSelect: true,
    sortName: 'name',
    onLoadSuccess: pathNav,
    onCheck: enable_fattr,
    onUncheck: disable_fattr,
    onDblClickRow: enter_directory,
    queryParams: setParams,
    columns: [{
      field: 'type',
      align: 'center',
      width: '5%',
      formatter: setIcon,
    }, {
      field: 'name',
      title: Name,
      halign: 'center',
      align: 'left',
      valign: 'middle',
      width: '58%',
      sortable: true,
      formatter: colorSet,
    }, {
      field: 'mtime',
      title: Mtime,
      align: 'center',
      valign: 'middle',
      sortable: true,
      width: '17%',
    }, {
      field: 'type',
      title: Type,
      align: 'center',
      valign: 'middle',
      sortable: true,
      width: '10%',
    }, {
      field: 'size',
      title: Size,
      halign: 'center',
      align: 'right',
      valign: 'middle',
      sortable: true,
      width: '10%',
      formatter: sizeConv,
    }, {
      checkbox: true,
      classes: "hide"
    }]
  });

  $("table#sys-attr").bootstrapTable({
    striped: true,
    classes: "table table-no-bordered modal-bs-middle the-table",
    columns: [{
      field: "key",
      title: Attribute,
      width: "50%",
      align: "center",
    }, {
      field: 'value',
      title: Value,
      width: "50%",
      align: "center",
      formatter: mutual,
    }]
  }).on('change', 'input#replica', function () {
    $(this).nextAll().find('button').removeAttr("disabled");
  }).on('change', ':checkbox', function() {
    $(this).parents('td').find('button').removeAttr("disabled");
  });

  $("table#user-attr").bootstrapTable({
    striped: true,
    classes: "table table-no-bordered modal-bs-middle",
    columns: [{
      field: "key",
      title: Attribute,
      width: "50%",
      align: "center",
    }, {
      field: 'value',
      title: Value,
      width: "50%",
      align: "center",
      formatter: mutual,
    }]
  });

  pathJson.bs = $("#filesys_content");
  browse_init();
}

/***** FileAttr *****/
function mutual(value, rowData) {
  var $tr = pathJson.bs.bootstrapTable("getSelections")[0],
      type = $tr.type,
      encode = $tr.transcode,
      html = '<div class="input-group input-group-sm" style="position: static;" data-animation="container: \'body\'">';
  if (rowData.key == Size) {
    if (type != Directory) {
      return value;
    } else {
      html += '<span class="form-control modal-fileattr" id="size">N/A</span>' +
        '<span class="input-group-btn">' +
        '<button class="btn btn-primary" type="button" onclick="setFattr(this,\'size\')">' +
        Calculate + '</button>' +
        '</span></div>';
    }
  } else if (rowData.key == ChunkSize) {
    if (type != Directory || !role_rw) {
      if (value == not_set) {
        return value;
      } else {
        return value + ' MB';
      }
    } else {
      html += '<input type="text" id="chunk" class="form-control pull-right" data-select="input" style="width: 65px;" value="' + (value || '8') + '">' +
        '<span class="input-group-addon" id="basic-addon2">MB</span>' +
        '<span class="input-group-btn">' +
        '<button class="btn btn-primary" type="button" onclick="setFattr(this,\'chunk\')"';

      if (encode) {
        html += ' disabled'
      };

      html += '>' + Site + '</button>' +
        '</span>' +
        '</div>' +
        '<div class="checkbox" style="text-align: right; line-height: 1;">' +
        '<label>' +
        '<input id="subdir" type="checkbox"';

      if (encode) {
        html += ' disabled'
      };

      html += '>' +
        '<span style="margin-left: 15px;">' + infomsg + '</span>' +
        '</label>' +
        '</div>';
    }
  } else if (rowData.key == Replevel) {
    if (!role_rw) {
      return value;
    }

    html += '<input type="text" class="form-control" id="replica" value="' + (value || '2x') + '" style="width: 50%; float: right; text-align: center;" readonly>' +
      '<span class="input-group-btn">' +
      '<button class="btn btn-primary" type="button" data-toggle="modal" data-target="#ProtectionModal"';

    if (encode) {
      html += ' disabled';
    };

    html += '>' + Site + '</button>' +
      '</span></div>';
  } else if (rowData.key == Path) {
    return "<span title=\"" + value + "\">" + value + "</span>";
  } else {
    return value;
  }

  return html;
}

var lastdata;
$("#fileattrModal").on("show.bs.modal", function() {
  if (lastdata == pathJson.bs.bootstrapTable("getSelections")[0])
    return;
  else {
    lastdata = pathJson.bs.bootstrapTable("getSelections")[0]
    var url = '/browse/fattr?path=' + lastdata.b64Path;
    $("#sys-attr").bootstrapTable('removeAll').bootstrapTable('showLoading');
    $("#user-attr").bootstrapTable('removeAll').bootstrapTable('showLoading');

    $.ajax({
      url: url,
      type: "GET",
      dataType: "JSON",
      success: function(info) {
        if (info.res) {
          file_type = info.sys[2].value;
          $("#sys-attr").bootstrapTable('hideLoading').bootstrapTable('load', info.sys);
          $("#user-attr").bootstrapTable('hideLoading').bootstrapTable('load', info.user);
          $input_rep_ec = $("input#replica");
          if (!lastdata.name.transcode) {
            level_path = lastdata.absPath;
          }
          if (lastdata.type == Directory) {
            $("#chunk").SelectTrunk();
          };
        }
      }
    });
  }
});

// File attribute button contorl
function setFattr(thebut, action) {
  $(thebut).text("Loading").attr("disabled", true);
  var selectData = pathJson.bs.bootstrapTable("getSelections")[0],
    data = {},
    url = "",
    type = "POST",
    async = false;


  if (action == "size") {
    type = "GET";
    url = "/browse/fattr/getDirSize?path=" + selectData.b64Path;
  } else {
    $(".append").hide();
    $("#chunk,#subdir").attr("disabled", "true");
    url = "/cli";
    data["type"] = "chunksize";
    data["path"] = selectData.absPath;
    data["value"] = $("#chunk").val();
    data["subdir"] = $("#subdir").prop("checked");
    data = JSON.stringify(data);
  }

  $.ajax({
    url: url,
    type: type,
    dataType: "JSON",
    data: data,
    success: function(info) {
      if (info.res) {
        if (action == "size") {
          $("#size").text(info.size);
          $(thebut).text("统计").removeAttr("disabled");
        } else
          $("#chunk").removeAttr("disabled");
          $(thebut).text("设置").removeAttr("disabled");
          $("button.append").show();
      }
    }
  });
}
