var reconnectAttempts = 0;
var maxReconnectAttempts = 20;
var reconnectDelayMs = 3000;
var reconnectTimer = null;
var updateMonitorTimer = null;
var uploadStarted = false;
var uploadReachedEnd = false;
var updateCompleted = false;
var wifiConnectInterval = null;

$(document).ready(function () {
  getSSID();
  setStatus("Ready", "ready");
  loadInitialStatus();
  startDHTSensorInterval();

  getLocalTime();
  startLocalTimeInterval();

  $("#ConnectInfo").hide();

  getConnectInfo();
  startWifiPageMonitor();

  $("#connect_wifi").on("click", function () {
    checkCredentials();
  });
  $("#disconnect_wifi").on("click", function () {
    disconnectWifi();
  });
});

function setStatus(text, type) {
  var badge = document.getElementById("status_badge");
  if (!badge) return;

  badge.textContent = text;
  badge.className = "status-badge";

  switch (type) {
    case "ready":
      badge.classList.add("status-ready");
      break;
    case "selected":
      badge.classList.add("status-selected");
      break;
    case "uploading":
      badge.classList.add("status-uploading");
      break;
    case "updating":
      badge.classList.add("status-updating");
      break;
    case "rebooting":
      badge.classList.add("status-rebooting");
      break;
    case "success":
      badge.classList.add("status-success");
      break;
    case "failed":
      badge.classList.add("status-failed");
      break;
    case "lost":
      badge.classList.add("status-lost");
      break;
    default:
      badge.classList.add("status-ready");
  }
}

function loadInitialStatus() {
  getUpdateStatus(
    function (response) {
      updateLatestFirmware(response);
      document.getElementById("ota_update_status").innerHTML =
        "Waiting for action";
      setStatus("Ready", "ready");
    },
    function () {
      document.getElementById("ota_update_status").innerHTML =
        "Device status unavailable.";
      setStatus("Connection Lost", "lost");
    },
  );
}

function getFileInfo() {
  var fileInput = document.getElementById("selected_file");

  if (fileInput.files && fileInput.files.length > 0) {
    var file = fileInput.files[0];

    document.getElementById("file_info").innerHTML =
      "File: " + file.name + "<br>Size: " + file.size + " bytes";

    document.getElementById("ota_update_status").innerHTML =
      "Firmware file selected and ready.";
    setStatus("File Selected", "selected");
  } else {
    document.getElementById("file_info").innerHTML = "No file selected";
    document.getElementById("ota_update_status").innerHTML =
      "Waiting for action";
    setStatus("Ready", "ready");
  }
}

function updateFirmware() {
  var formData = new FormData();
  var fileSelect = document.getElementById("selected_file");

  clearAllTimers();
  reconnectAttempts = 0;
  uploadStarted = false;
  uploadReachedEnd = false;
  updateCompleted = false;

  if (fileSelect.files && fileSelect.files.length === 1) {
    var file = fileSelect.files[0];
    formData.set("file", file, file.name);

    uploadStarted = true;

    document.getElementById("ota_update_status").innerHTML =
      "Uploading " + file.name + "...";
    setStatus("Uploading", "uploading");

    var request = new XMLHttpRequest();

    request.upload.addEventListener("progress", function (oEvent) {
      if (oEvent.lengthComputable) {
        var percent = Math.round((oEvent.loaded / oEvent.total) * 100);

        document.getElementById("ota_update_status").innerHTML =
          "Uploading firmware... " + percent + "%";
        setStatus("Uploading", "uploading");

        if (percent >= 100) {
          uploadReachedEnd = true;
        }
      } else {
        document.getElementById("ota_update_status").innerHTML =
          "Uploading firmware...";
        setStatus("Uploading", "uploading");
      }
    });

    request.onload = function () {
      console.log("OTA request status:", request.status);
      console.log("OTA response text:", request.responseText);

      if (request.status === 200) {
        document.getElementById("ota_update_status").innerHTML =
          request.responseText ||
          "Update completed successfully. Device will reboot.";
        setStatus("Rebooting", "rebooting");

        setTimeout(monitorReconnectOnly, 1500);
      } else {
        document.getElementById("ota_update_status").innerHTML =
          "Upload failed. Server returned status: " + request.status;
        setStatus("Failed", "failed");
      }
    };

    request.onerror = function () {
      if (uploadStarted && uploadReachedEnd) {
        document.getElementById("ota_update_status").innerHTML =
          "Update completed successfully. Device connection may be temporarily unavailable while it reboots and reconnects.";
        setStatus("Rebooting", "rebooting");

        setTimeout(monitorReconnectOnly, 1500);
      } else {
        document.getElementById("ota_update_status").innerHTML =
          "Upload failed due to network or server error.";
        setStatus("Failed", "failed");
      }
    };

    request.open("POST", "/OTAupdate", true);
    request.send(formData);
  } else {
    alert("Select A File First");
    document.getElementById("ota_update_status").innerHTML =
      "No firmware file selected.";
    setStatus("Failed", "failed");
  }
}

function monitorReconnectOnly() {
  reconnectAttempts++;

  getUpdateStatus(
    function (response) {
      updateCompleted = true;
      updateLatestFirmware(response);

      document.getElementById("ota_update_status").innerHTML =
        "Device reconnected successfully. Reloading updated web interface...";
      setStatus("Connected", "success");

      setTimeout(function () {
        window.location.replace(window.location.origin + "/?t=" + Date.now());
      }, 10000);
    },
    function () {
      if (reconnectAttempts < maxReconnectAttempts) {
        document.getElementById("ota_update_status").innerHTML =
          "Update completed successfully. Device is reconnecting automatically... Attempt " +
          reconnectAttempts +
          " of " +
          maxReconnectAttempts +
          ".";
        setStatus("Rebooting", "rebooting");

        reconnectTimer = setTimeout(monitorReconnectOnly, reconnectDelayMs);
      } else {
        document.getElementById("ota_update_status").innerHTML =
          "Update completed successfully, but the device is still unreachable from this page. It may still be rebooting or reconnecting, or the connection may have been interrupted.";
        setStatus("Connection Lost", "lost");
      }
    },
  );
}

function getUpdateStatus(onSuccess, onError) {
  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function () {
    if (xhr.readyState === 4) {
      if (xhr.status === 200) {
        try {
          var response = JSON.parse(xhr.responseText);
          if (onSuccess) onSuccess(response);
        } catch (e) {
          if (onError) onError();
        }
      } else {
        if (onError) onError();
      }
    }
  };

  xhr.onerror = function () {
    if (onError) onError();
  };

  xhr.open("POST", "/OTAstatus", true);
  xhr.send("ota_update_status");
}

function updateLatestFirmware(response) {
  if (
    response &&
    typeof response.compile_date !== "undefined" &&
    typeof response.compile_time !== "undefined"
  ) {
    document.getElementById("latest_firmware").innerHTML =
      response.compile_date + " - " + response.compile_time;
  }
}

function clearAllTimers() {
  if (reconnectTimer) {
    clearTimeout(reconnectTimer);
    reconnectTimer = null;
  }

  if (updateMonitorTimer) {
    clearTimeout(updateMonitorTimer);
    updateMonitorTimer = null;
  }
}

/*
 * Gets DHT22 sensor temperature and humidity values for display on the web page
 */
function getDHTSensorValues() {
  $.getJSON("/dhtSensor.json")
    .done(function (data) {
      $("#temperature_readings").text(data.temp + " °C");
      $("#humidity_readings").text(data.humidity + " %");
      $("#sensor_status").text("Sensor data updated successfully.");
    })
    .fail(function () {
      $("#sensor_status").text("Failed to read sensor data.");
    });
}

/*
 * Sets the interval for getting the updated DHT22 sensor values
 */
function startDHTSensorInterval() {
  getDHTSensorValues(); // first read immediately
  setInterval(getDHTSensorValues, 5000);
}

/*
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval() {
  if (wifiConnectInterval != null) {
    clearInterval(wifiConnectInterval);
    wifiConnectInterval = null;
  }
}

/*
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus() {
  var xhr = new XMLHttpRequest();
  var requestURL = "/wifiConnectStatus.json";

  xhr.onreadystatechange = function () {
    if (xhr.readyState == 4 && xhr.status == 200) {
      var response = JSON.parse(xhr.responseText);

      if (response.wifi_connect_status == 0) {
        document.getElementById("wifi_connect_status").innerHTML =
          "Waiting for action";
        clearConnectInfo();
      } else if (response.wifi_connect_status == 1) {
        document.getElementById("wifi_connect_status").innerHTML =
          "Connecting...";
      } else if (response.wifi_connect_status == 2) {
        document.getElementById("wifi_connect_status").innerHTML =
          "<span class='rd'>Failed to connect. Retrying...</span>";
        clearConnectInfo();
      } else if (response.wifi_connect_status == 3) {
        document.getElementById("wifi_connect_status").innerHTML =
          "<span class='gr'>Connection Success!</span>";

        stopWifiConnectStatusInterval();
        getConnectInfo();
      } else if (response.wifi_connect_status == 4) {
        document.getElementById("wifi_connect_status").innerHTML =
          "<span class='rd'>Disconnected</span>";
        stopWifiConnectStatusInterval();
        clearConnectInfo();
      }
    }
  };

  xhr.onerror = function () {
    document.getElementById("wifi_connect_status").innerHTML =
      "<span class='rd'>Connection error</span>";
  };

  xhr.open("POST", requestURL, true);
  xhr.send("wifi_connect_status");
}

/*
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval() {
  wifiConnectInterval = setInterval(getWifiConnectStatus, 5000);
}

/*
 * Connect WiFi function called using the SSID and Password entered into the text fields.
 */
function connectWifi() {
  var selectedSSID = $("#connect_ssid").val();
  var pwd = $("#connect_pass").val();

  clearConnectInfo();

  $.ajax({
    url: "/wifiConnect.json",
    dataType: "json",
    method: "POST",
    cache: false,
    headers: { "my-connect-ssid": selectedSSID, "my-connect-pwd": pwd },
    data: { timestamp: Date.now() },
  });

  $("#wifi_connect_status").html("Connecting...");
  stopWifiConnectStatusInterval();
  startWifiConnectStatusInterval();
}

/*
 * Checks credentials on connect WiFi button click.
 */
function checkCredentials() {
  var errorList = "";
  var credsOK = true;

  var selectedSSID = $("#connect_ssid").val();
  var pwd = $("#connect_pass").val();

  if (selectedSSID == "") {
    errorList += "<div class='rd'>SSID cannot be empty!</div>";
    credsOK = false;
  }

  if (pwd == "") {
    errorList += "<div class='rd'>Password cannot be empty!</div>";
    credsOK = false;
  }

  if (credsOK == false) {
    $("#wifi_connect_credentials_errors").html(errorList);
  } else {
    $("#wifi_connect_credentials_errors").html("");
    connectWifi();
  }
}

/*
 * Shows the WiFi password if the box is checked.
 */
function showPassword() {
  var x = document.getElementById("connect_pass");
  if (x.type === "password") {
    x.type = "text";
  } else {
    x.type = "password";
  }
}

/**
 * Gets the connection information for displaying on the web page.
 */
function getConnectInfo() {
  $.getJSON("/wifiConnectInfo.json")
    .done(function (data) {
      if (data && data.ap && data.ip && data.netmask && data.gw) {
        $("#connected_ap").text(data.ap);
        $("#wifi_connect_ip").text(data.ip);
        $("#wifi_connect_netmask").text(data.netmask);
        $("#wifi_connect_gw").text(data.gw);

        $("#ConnectInfo").show();
        $("#disconnect_wifi").show();
      } else {
        $("#ConnectInfo").hide();
      }
    })
    .fail(function () {
      $("#ConnectInfo").hide();
    });
}
function clearConnectInfo() {
  $("#connected_ap").text("--");
  $("#wifi_connect_ip").text("--");
  $("#wifi_connect_netmask").text("--");
  $("#wifi_connect_gw").text("--");
  $("#ConnectInfo").hide();
}

function disconnectWifi() {
  $.ajax({
    url: "/wifiDisConnect.json",
    method: "DELETE",
    cache: false,
    data: { timestamp: Date.now() },

    success: function () {
      $("#wifi_connect_status").html("Disconnecting...");
      clearConnectInfo();
    },

    error: function (xhr, status, error) {
      console.log("Disconnect error:", status, error);
      $("#wifi_connect_status").html("Disconnect request failed.");
    },
  });
}

function startWifiPageMonitor() {
  setInterval(function () {
    getWifiConnectStatus();
    getConnectInfo();
  }, 5000);
}

/**
 * Sets the interval for displaying local time
 */
function startLocalTimeInterval() {
  setInterval(getLocalTime, 10000);
}

/**
 * Gets the local time.
 * @note connect the ESP32 to the internet and the time will be updated.
 */
function getLocalTime() {
  $.getJSON("/localTime.json", function (data) {
    $("#local_time").text(data["time"]);
  });
}

/**
 * Gets the ESP32's access point SSID for displaying on the web page.
 */
function getSSID() {
  $.getJSON("/apSSID.json", function (data) {
    $("#ap_ssid").text(data["ssid"]);
  });
}
