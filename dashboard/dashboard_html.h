#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H
#include <Arduino.h>
const char DASHBOARD_HTML_BODY[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>S5CSE A — Smart Classroom</title>
  <style>__INJECT_CSS__</style>
</head>
<body>
  <div class="container">
    <div id="emergencyBanner">
      <span style="font-size: 1.8rem;">🚨</span>
      <div>
        <div style="font-size: 1.1rem; letter-spacing: 0.04em;">HAZARD ALERT: SMOKE DETECTED IN S5CSE A CLASSROOM!</div>
        <div style="font-size: 0.85rem; font-weight: 500; opacity: 0.9;">Emergency exhaust ventilation relay activated and buzzer alarm is sounding.</div>
      </div>
    </div>
    <header>
      <div class="brand-title">
        <div class="brand-icon">🏫</div>
        <div>
          <h1>S5CSE A</h1>
          <div class="subtitle">Smart Classroom Automation</div>
        </div>
      </div>
      <div class="header-meta">
        <div class="badge badge-active">
          <div class="badge-pulse"></div>
          <span>LIVE</span>
        </div>
        <div class="badge">
          <span>IP:</span> <strong id="badgeIp">192.168.x.x</strong>
        </div>
        <div class="badge">
          <span>Wi-Fi:</span> <span id="badgeSsid">Scanning...</span>
        </div>
        <div class="badge">
          <span>Devices:</span> <strong id="badgeConnectedCount">--</strong>
        </div>
        <div class="badge">
          <span>Uptime:</span> <span id="badgeUptime">00:00:00</span>
        </div>
      </div>
    </header>
    <div class="section-title">Classroom Environment</div>
    <div class="grid-env">
      <div class="card">
        <div class="card-top">
          <span class="card-label">Temperature</span>
          <span class="card-icon">🌡️</span>
        </div>
        <div class="card-value"><span id="valTemp">--</span><span class="card-unit">°C</span></div>
        <div class="card-subtext">DHT11 Climate Sensor</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Humidity</span>
          <span class="card-icon">💧</span>
        </div>
        <div class="card-value"><span id="valHum">--</span><span class="card-unit">%</span></div>
        <div class="card-subtext">Relative Indoor Humidity</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Ambient Light</span>
          <span class="card-icon">☀️</span>
        </div>
        <div class="card-value"><span id="valLight">--</span><span class="card-unit">ADC</span></div>
        <div class="card-subtext" id="subLight">Optical LDR Sensor</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Air & Smoke</span>
          <span id="pillSmoke" class="status-pill status-on">NORMAL</span>
        </div>
        <div class="card-value"><span id="valSmoke">--</span><span class="card-unit">ADC</span></div>
        <div class="card-subtext">MQ-2 Environmental Gas Sensor</div>
      </div>
    </div>
    <div class="section-title" style="display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 0.6rem;">
      <div style="display: flex; align-items: center; gap: 0.6rem;">
        <span>Connected Devices</span>
        <span class="badge" style="background: rgba(56, 189, 248, 0.15); color: var(--accent-cyan); font-size: 0.8rem; border: 1px solid rgba(56, 189, 248, 0.3);">
          <span id="lblConnectedSummary">0 Devices Active</span>
        </span>
      </div>
      <div class="device-filter-pills">
        <button class="filter-pill active" id="btnFilterAll" onclick="filterDevices('all')">
          <span>All Devices</span> (<span id="countPillAll">0</span>)
        </button>
        <button class="filter-pill" id="btnFilterWifi" onclick="filterDevices('wifi')">
          <span>📱 Wi-Fi Clients</span> (<span id="countPillWifi">0</span>)
        </button>
        <button class="filter-pill" id="btnFilterHw" onclick="filterDevices('hardware')">
          <span>⚙️ Hardware</span> (<span id="countPillHw">0</span>)
        </button>
      </div>
    </div>
    <div class="grid-env" style="margin-bottom: 1.25rem;">
      <div class="card">
        <div class="card-top">
          <span class="card-label">Total Connected</span>
          <span class="card-icon">🔌</span>
        </div>
        <div class="card-value"><span id="valTotalDevices">0</span><span class="card-unit" style="font-size: 0.95rem; color: var(--text-muted); margin-left: 0.4rem;">devices</span></div>
        <div class="card-subtext">Active network &amp; physical devices</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Wi-Fi Client Devices</span>
          <span class="card-icon">📡</span>
        </div>
        <div class="card-value"><span id="valWifiDevices">0</span><span class="card-unit" style="font-size: 0.95rem; color: var(--text-muted); margin-left: 0.4rem;">connected</span></div>
        <div class="card-subtext" id="subWifiDesc">Phones &amp; PCs on classroom AP</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Hardware Peripherals</span>
          <span class="card-icon">🎛️</span>
        </div>
        <div class="card-value"><span id="valHwDevices">10</span><span class="card-unit" style="font-size: 0.95rem; color: var(--text-muted); margin-left: 0.4rem;">online</span></div>
        <div class="card-subtext">Sensors, relays &amp; micro-servos</div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Network Interface</span>
          <span class="card-icon">📶</span>
        </div>
        <div class="card-value" style="font-size: 1.25rem; font-weight: 700; color: var(--accent-cyan);"><span id="valNetMode">SoftAP Mode</span></div>
        <div class="card-subtext" id="subNetDetail">SSID: S5CSEA (192.168.4.1)</div>
      </div>
    </div>
    <div class="devices-grid" id="devicesListContainer" style="margin-bottom: 2rem;">
      <!-- Dynamically generated device cards -->
    </div>
    <div id="renameDeviceModal" class="modal-overlay" style="display: none;">
      <div class="modal-card">
        <div class="modal-header">
          <h3 style="font-size: 1.15rem; font-weight: 700; color: #F1F5F9;">✏️ Rename Connected Device</h3>
          <button class="modal-close-btn" onclick="closeRenameModal()">&times;</button>
        </div>
        <div class="modal-body">
          <p style="font-size: 0.85rem; color: var(--text-muted); margin-bottom: 1rem;">
            Assign a custom name to this device (e.g., <em>Abin's Phone</em> or <em>Teacher's Laptop</em>):
          </p>
          <input type="hidden" id="modalDeviceIp" value="">
          <div style="margin-bottom: 1rem;">
            <label style="display: block; font-size: 0.78rem; font-weight: 600; color: var(--text-muted); margin-bottom: 0.4rem; letter-spacing: 0.05em;">DEVICE NAME</label>
            <input type="text" id="modalDeviceName" class="modal-input" placeholder="e.g. Abin's Galaxy S24" maxlength="32">
          </div>
          <div style="font-size: 0.8rem; color: var(--text-muted); margin-bottom: 1.25rem;">
            Device IP: <strong id="modalIpDisplay" style="color: var(--accent-cyan); font-family: monospace;">--</strong>
          </div>
          <div style="display: flex; gap: 0.75rem; justify-content: flex-end;">
            <button class="btn" onclick="closeRenameModal()" style="border: 1px solid var(--border-subtle); padding: 0.5rem 1rem;">Cancel</button>
            <button class="btn btn-emerald" onclick="submitDeviceRename()" style="padding: 0.5rem 1.25rem;">Save Name</button>
          </div>
        </div>
      </div>
    </div>
    <div class="section-title">Classroom Automation & Actuators</div>
    <div class="grid-controls">
      <div class="card">
        <div class="card-top">
          <span class="card-label">Classroom Lighting</span>
          <span id="pillLight" class="status-pill status-off">OFF</span>
        </div>
        <div style="font-size: 0.88rem; color: var(--text-muted); margin-bottom: 0.5rem;">
          GPIO 15 Opto-Relay • Automatic LDR Sync
        </div>
        <div class="btn-group">
          <button id="btnLightAuto" class="btn btn-light active" onclick="setLight('auto')">AUTO</button>
          <button id="btnLightOn" class="btn btn-light btn-emerald" onclick="setLight('on')">ON</button>
          <button id="btnLightOff" class="btn btn-light" onclick="setLight('off')">OFF</button>
        </div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Ventilation Fan</span>
          <span id="pillFan" class="status-pill status-off">OFF</span>
        </div>
        <div style="font-size: 0.88rem; color: var(--text-muted); margin-bottom: 0.5rem;">
          GPIO 16 Opto-Relay • Thermal Automation
        </div>
        <div class="btn-group">
          <button id="btnFanAuto" class="btn btn-fan active" onclick="setFan('auto')">AUTO</button>
          <button id="btnFanOn" class="btn btn-fan btn-emerald" onclick="setFan('on')">ON</button>
          <button id="btnFanOff" class="btn btn-fan" onclick="setFan('off')">OFF</button>
        </div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Motorized Windows</span>
          <span id="pillWindow" class="status-pill status-off">CLOSED</span>
        </div>
        <div style="font-size: 0.88rem; color: var(--text-muted); margin-bottom: 0.5rem;">
          GPIO 11 &amp; 10 — Dual Servo PWM (ESP32-S3 N16R8)
          &nbsp;| State: <span id="valWindowState" style="color: var(--accent-cyan);">CLOSED</span>
        </div>
        <div style="display: flex; gap: 0.5rem; margin-top: 0.8rem;">
          <button id="btnWinOpen" class="btn btn-emerald" style="border: 1px solid rgba(16,185,129,0.3);" onclick="setWindow('open')">OPEN BOTH</button>
          <button id="btnWinClose" class="btn" style="border: 1px solid var(--border-subtle);" onclick="setWindow('close')">CLOSE BOTH</button>
        </div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Door Access & RFID</span>
          <span id="pillDoor" class="status-pill status-off">LOCKED</span>
        </div>
        <div style="font-size: 0.88rem; color: var(--text-muted); margin-bottom: 0.5rem;">
          PN532 SPI • Dual-Authentication
        </div>
        <div id="rfidLiveBox" class="rfid-live-box">
          <div class="rfid-header">
            <span class="rfid-title">
              <span>💳</span>
              <span>RFID Scanner</span>
            </span>
            <span id="rfidStatusPill" class="status-pill status-off">IDLE</span>
          </div>
          <div class="rfid-name-row">
            <div class="rfid-name" id="rfidNameDisplay">Waiting for card...</div>
            <div class="rfid-uid-chip" id="rfidUidDisplay">-- -- -- --</div>
          </div>
          <div class="rfid-desc">
            <span id="rfidDescIcon">📡</span>
            <span id="rfidDescText">Present RFID student card to reader</span>
            <span id="rfidTimeAgo" class="rfid-timeago" style="margin-left: auto;"></span>
          </div>
        </div>
        <div class="btn-group">
          <button id="btnDoorOpen" class="btn btn-emerald" style="background: rgba(16,185,129,0.15); color: #34D399;" onclick="setDoor('open')">UNLOCK / OPEN</button>
          <button id="btnDoorLock" class="btn" style="background: rgba(239,68,68,0.15); color: #F87171;" onclick="setDoor('lock')">LOCK DOOR</button>
        </div>
      </div>
    </div>
    <div class="section-title">Hardware I²C OLED Message Display</div>
    <div class="card" style="margin-bottom: 2rem;">
      <div class="card-top">
        <div>
          <span class="card-label">Physical 0.96" SH1106 OLED (128×64)</span>
          <div style="font-size: 0.8rem; color: var(--text-muted); margin-top: 0.25rem;">
            Direct Peripheral Bus &bull; SDA: GPIO 39 &bull; SCL: GPIO 40 &bull; U8g2 SH1106
          </div>
        </div>
        <span id="pillOledHw" class="status-pill status-on">I²C READY</span>
      </div>
      <div class="notice-input-box" style="margin-top: 0.75rem;">
        <textarea id="noticeInput" placeholder="Type message to display directly on physical OLED hardware (e.g. WELCOME TO S5CSE A)..."></textarea>
        <div style="display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 0.75rem;">
          <div style="font-size: 0.85rem; color: var(--text-muted);">
            Active Hardware Message: <strong id="lblActiveNotice" style="color: var(--accent-cyan); font-family: monospace;">Waiting for IoT message...</strong>
          </div>
          <button id="btnPublishNotice" class="btn-submit" style="width: auto; min-width: 240px;" onclick="publishNotice()">
            🚀 Send to Hardware I²C OLED
          </button>
        </div>
      </div>
    </div>
    <div class="section-title">Class Logs</div>
    <div class="grid-controls">
      <div class="card">
        <div class="card-top">
          <span class="card-label">Student Attendance</span>
          <span class="card-icon">📋</span>
        </div>
        <div id="attendanceStatus" class="attendance-status">
          <div class="attendance-row">
            <span class="attendance-name">Rahul</span>
            <span class="status-pill status-off">Absent</span>
          </div>
          <div class="attendance-row">
            <span class="attendance-name">Laksmi</span>
            <span class="status-pill status-off">Absent</span>
          </div>
        </div>
      </div>
      <div class="card">
        <div class="card-top">
          <span class="card-label">Attendance Event Log</span>
          <span class="card-icon">🕐</span>
        </div>
        <div id="attendanceLog" class="attendance-log">
          <div class="attendance-log-empty">No attendance events recorded yet.</div>
        </div>
      </div>
    </div>
    <div class="section-title" style="display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 0.5rem; margin-top: 2rem;">
      <div style="display: flex; align-items: center; gap: 0.5rem;">
        <span>Live Serial Monitor &amp; Sensor Telemetry</span>
        <span class="badge" style="background: rgba(16,185,129,0.15); color: #34D399; font-size: 0.75rem; border: 1px solid rgba(16,185,129,0.3);">115200 BAUD</span>
      </div>
      <div style="font-size: 0.8rem; font-weight: 500; color: var(--text-muted);">
        Real-time sensor logs categorized for instant diagnosis
      </div>
    </div>
    <div class="card terminal-card">
      <div class="terminal-tabs-header">
        <div class="terminal-tabs" id="terminalTabs">
          <button class="terminal-tab active" data-cat="all" onclick="setLogCategory('all')">
            <span>🔘 All Logs</span>
            <span class="tab-count" id="countAll">0</span>
          </button>
          <button class="terminal-tab" data-cat="rfid" onclick="setLogCategory('rfid')">
            <span>🚪 RFID &amp; Door</span>
            <span class="tab-count" id="countRfid">0</span>
          </button>
          <button class="terminal-tab" data-cat="smoke" onclick="setLogCategory('smoke')">
            <span>🔥 Smoke (MQ-2)</span>
            <span class="tab-count" id="countSmoke">0</span>
          </button>
          <button class="terminal-tab" data-cat="climate" onclick="setLogCategory('climate')">
            <span>🌡️ Climate &amp; Fan</span>
            <span class="tab-count" id="countClimate">0</span>
          </button>
          <button class="terminal-tab" data-cat="light" onclick="setLogCategory('light')">
            <span>☀️ Light (LDR)</span>
            <span class="tab-count" id="countLight">0</span>
          </button>
          <button class="terminal-tab" data-cat="window" onclick="setLogCategory('window')">
            <span>🪟 Windows</span>
            <span class="tab-count" id="countWindow">0</span>
          </button>
          <button class="terminal-tab" data-cat="system" onclick="setLogCategory('system')">
            <span>⚙️ System</span>
            <span class="tab-count" id="countSystem">0</span>
          </button>
        </div>
        <div class="terminal-controls">
          <div class="search-box">
            <span>🔍</span>
            <input type="text" id="logSearchInput" placeholder="Filter logs..." oninput="renderLogs()">
          </div>
          <label class="toggle-autoscroll">
            <input type="checkbox" id="chkAutoScroll" checked>
            <span>Auto-Scroll</span>
          </label>
          <button id="btnPauseLogs" class="btn-console" onclick="togglePauseLogs()">
            <span id="txtPauseLogs">⏸ Pause</span>
          </button>
          <button class="btn-console" onclick="clearLogsOnServer()">
            <span>🗑 Clear</span>
          </button>
        </div>
      </div>
      <div class="terminal-screen" id="terminalScreen">
        <div class="terminal-line system-welcome">
          <span class="log-time">[BOOT]</span>
          <span class="log-badge badge-system">SYSTEM</span>
          <span class="log-msg">S5CSE A Smart Classroom Serial Monitor Stream Connected...</span>
        </div>
      </div>
      <div class="terminal-footer">
        <div style="display: flex; gap: 1rem; align-items: center;">
          <span>Showing: <strong id="visibleLogCount" style="color: var(--accent-cyan);">0</strong> lines</span>
          <span>Category: <strong id="activeCatName" style="color: #F1F5F9;">All Subsystems</strong></span>
        </div>
        <div style="display: flex; gap: 0.75rem;">
          <span style="color: var(--text-muted);">ESP32-S3 Direct Hardware Stream</span>
        </div>
      </div>
    </div>
    <footer>
      <div>
        <strong>S5CSE A Smart Classroom Controller</strong> • Powered by ESP32-S3 N16R8
      </div>
      <div>
        <span>Live Classroom Control & Automation</span>
      </div>
    </footer>
  </div>
  <script>__INJECT_JS__</script>
</body>
</html>
)rawliteral";
const char CAPTIVE_PORTAL_HTML_BODY[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="refresh" content="0; url=/">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>S5CSE A — Redirecting...</title>
  <script>
    window.location.replace("/");
  </script>
  <style>
    body {
      background: #0B1120;
      color: #F1F5F9;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
      padding: 1.5rem;
    }
    .box {
      background: rgba(30, 41, 59, 0.85);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 16px;
      padding: 2rem;
      max-width: 420px;
      width: 100%;
      text-align: center;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
    }
    h2 { font-size: 1.5rem; margin-bottom: 0.25rem; }
    .sub { font-size: 0.85rem; color: #38BDF8; font-weight: 600; text-transform: uppercase; margin-bottom: 1.5rem; }
    .btn-link {
      display: inline-block;
      margin-top: 1rem;
      padding: 0.75rem 1.25rem;
      background: #2563EB;
      color: #FFFFFF;
      text-decoration: none;
      font-weight: 700;
      border-radius: 8px;
    }
  </style>
</head>
<body>
  <div class="box">
    <h2>S5CSE A</h2>
    <div class="sub">Smart Classroom Automation</div>
    <p>Opening Classroom Dashboard...</p>
    <a href="/" class="btn-link">Open Dashboard</a>
  </div>
</body>
</html>
)rawliteral";
#endif  
