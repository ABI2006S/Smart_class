#ifndef DASHBOARD_JS_H
#define DASHBOARD_JS_H
#include <Arduino.h>
const char DASHBOARD_JS[] PROGMEM = R"rawliteral(
let allLogs = [];
let activeLogCategory = 'all';
let isLogsPaused = false;
let logSeenSet = new Set();
let activeDeviceFilter = 'all';
let cachedWifiDevices = [];
let cachedHwDevices = [];

function getDeviceIcon(type, category, name) {
  const n = (name || '').toLowerCase();
  const t = (type || '').toLowerCase();
  const c = (category || '').toLowerCase();
  if (t === 'mobile' || n.includes('phone')) return '📱';
  if (t === 'tablet' || n.includes('ipad')) return '📟';
  if (t === 'laptop' || n.includes('macbook') || n.includes('chromebook')) return '💻';
  if (t === 'desktop' || n.includes('pc')) return '🖥️';
  if (c === 'sensor') {
    if (n.includes('climate') || n.includes('dht')) return '🌡️';
    if (n.includes('smoke') || n.includes('gas') || n.includes('mq-2')) return '🔥';
    if (n.includes('light') || n.includes('ldr')) return '☀️';
    return '📡';
  }
  if (c === 'actuator') {
    if (n.includes('light')) return '💡';
    if (n.includes('fan')) return '🌀';
    if (n.includes('window')) return '🪟';
    if (n.includes('door')) return '🚪';
    return '⚡';
  }
  if (c === 'safety') return '🚨';
  if (c === 'access') return '💳';
  if (c === 'display') return '📺';
  return '🔌';
}

function filterDevices(filter) {
  activeDeviceFilter = filter;
  const pills = document.querySelectorAll('.device-filter-pills .filter-pill');
  pills.forEach(b => b.classList.remove('active'));
  const btnAll = document.getElementById('btnFilterAll');
  const btnWifi = document.getElementById('btnFilterWifi');
  const btnHw = document.getElementById('btnFilterHw');
  if (filter === 'all' && btnAll) btnAll.classList.add('active');
  else if (filter === 'wifi' && btnWifi) btnWifi.classList.add('active');
  else if (filter === 'hardware' && btnHw) btnHw.classList.add('active');
  renderDevices();
}

function renderDevices() {
  const container = document.getElementById('devicesListContainer');
  if (!container) return;
  let items = [];

  if (activeDeviceFilter === 'all' || activeDeviceFilter === 'wifi') {
    cachedWifiDevices.forEach(d => {
      items.push({
        isWifi: true,
        name: d.name || 'Wi-Fi Client',
        type: d.type || 'Client',
        ip: d.ip || '--',
        mac: d.mac || '--',
        category: 'Wi-Fi Network Client',
        pinOrIp: d.ip ? `IP: ${d.ip}` : (d.mac ? `MAC: ${d.mac}` : 'Wi-Fi'),
        status: d.active !== false ? 'ONLINE' : 'STANDBY',
        statusClass: d.active !== false ? 'status-on' : 'status-off',
        reading: d.lastSeenAgo !== undefined ? (d.lastSeenAgo <= 5 ? 'Active now' : `${d.lastSeenAgo}s ago`) : 'Active'
      });
    });
  }

  if (activeDeviceFilter === 'all' || activeDeviceFilter === 'hardware') {
    cachedHwDevices.forEach(h => {
      let statClass = 'status-on';
      if (h.status === 'FAULT' || h.status === 'ALERT') statClass = 'status-danger';
      else if (h.status === 'OFFLINE') statClass = 'status-off';
      items.push({
        isWifi: false,
        name: h.name,
        type: h.category,
        category: h.category,
        pinOrIp: h.pin,
        status: h.status || 'ONLINE',
        statusClass: statClass,
        reading: h.reading || 'Operational'
      });
    });
  }

  if (items.length === 0) {
    container.innerHTML = '<div style="grid-column: 1 / -1; padding: 2rem; text-align: center; color: var(--text-muted); background: var(--bg-card); border-radius: var(--radius-md); border: 1px solid var(--border-subtle);">No connected devices found in this view.</div>';
    return;
  }

  let html = '';
  items.forEach(item => {
    const icon = getDeviceIcon(item.type, item.category, item.name);
    const renameBtn = item.isWifi && item.ip && item.ip !== '--'
      ? `<button class="device-btn-rename" onclick="openRenameModal('${item.ip}', '${item.name.replace(/'/g, "\\'")}')">✏️ Rename</button>`
      : '';
    html += `
      <div class="device-item-card">
        <div class="device-card-head">
          <div class="device-title-area">
            <div class="device-icon-box">${icon}</div>
            <div>
              <div class="device-name-text" title="${item.name}">${item.name}</div>
              <div class="device-cat-badge">${item.category}</div>
            </div>
          </div>
          <span class="status-pill ${item.statusClass}">${item.status}</span>
        </div>
        <div class="device-meta-row">
          <span>Interface / Pin</span>
          <span class="device-meta-val">${item.pinOrIp}</span>
        </div>
        <div class="device-meta-row">
          <span>State / Telemetry</span>
          <span style="color: var(--accent-cyan); font-weight: 500;">${item.reading}</span>
        </div>
        ${renameBtn ? `<div style="display: flex; justify-content: flex-end; margin-top: 0.2rem;">${renameBtn}</div>` : ''}
      </div>
    `;
  });
  container.innerHTML = html;
}

function openRenameModal(ip, currentName) {
  const modal = document.getElementById('renameDeviceModal');
  const ipInput = document.getElementById('modalDeviceIp');
  const ipDisp = document.getElementById('modalIpDisplay');
  const nameInput = document.getElementById('modalDeviceName');
  if (ipInput) ipInput.value = ip;
  if (ipDisp) ipDisp.innerText = ip;
  if (nameInput) nameInput.value = currentName || '';
  if (modal) modal.style.display = 'flex';
  setTimeout(() => {
    if (nameInput) nameInput.focus();
  }, 100);
}

function closeRenameModal() {
  const modal = document.getElementById('renameDeviceModal');
  if (modal) modal.style.display = 'none';
}

async function submitDeviceRename() {
  const ip = document.getElementById('modalDeviceIp').value;
  const name = document.getElementById('modalDeviceName').value.trim();
  if (!name) return;
  try {
    await sendPost('/api/device/rename', `ip=${encodeURIComponent(ip)}&name=${encodeURIComponent(name)}`);
    localStorage.setItem('smartclass_device_name', name);
    closeRenameModal();
    updateDashboard();
  } catch (e) {
    console.error('Rename failed', e);
  }
}

function registerSelf() {
  let devName = localStorage.getItem('smartclass_device_name');
  if (!devName) {
    const ua = navigator.userAgent;
    let platform = 'Client Device';
    if (/iPhone/i.test(ua)) platform = 'Apple iPhone';
    else if (/iPad/i.test(ua)) platform = 'Apple iPad';
    else if (/Android/i.test(ua)) platform = /Mobile/i.test(ua) ? 'Android Phone' : 'Android Tablet';
    else if (/Windows/i.test(ua)) platform = 'Windows PC';
    else if (/Macintosh/i.test(ua)) platform = 'Apple Mac';
    else if (/Linux/i.test(ua)) platform = 'Linux Device';
    else if (/CrOS/i.test(ua)) platform = 'Chromebook';
    devName = platform;
  }
  fetch('/api/device/register', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: 'name=' + encodeURIComponent(devName)
  }).catch(() => {});
}

function formatUptime(seconds) {
  const d = Math.floor(seconds / 86400);
  const h = Math.floor((seconds % 86400) / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  if (d > 0) return `${d}d ${h}h ${m}m`;
  return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
}
function formatMillis(ms) {
  const totalSec = Math.floor(ms / 1000);
  const h = String(Math.floor(totalSec / 3600)).padStart(2, '0');
  const m = String(Math.floor((totalSec % 3600) / 60)).padStart(2, '0');
  const s = String(totalSec % 60).padStart(2, '0');
  return `[${h}:${m}:${s}]`;
}
function highlightLog(msg) {
  let safe = msg.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  safe = safe.replace(/\b(AUTHORIZED|GRANTED|OPEN|NORMAL|ON|OK|READY|CONNECTED)\b/g, '<span class="kw-good">$1</span>');
  safe = safe.replace(/\b(DENIED|UNAUTHORIZED|HAZARD|ALERT|CLOSED|LOCKED|OFF|ERROR|FAILED)\b/g, '<span class="kw-bad">$1</span>');
  return safe;
}
function renderLogs() {
  const screen = document.getElementById('terminalScreen');
  const search = (document.getElementById('logSearchInput').value || '').toLowerCase();
  const autoScroll = document.getElementById('chkAutoScroll').checked;
  let filtered = allLogs.filter(log => {
    if (activeLogCategory !== 'all' && log.cat.toLowerCase() !== activeLogCategory.toLowerCase()) {
      return false;
    }
    if (search && !log.msg.toLowerCase().includes(search) && !log.cat.toLowerCase().includes(search)) {
      return false;
    }
    return true;
  });
  document.getElementById('visibleLogCount').innerText = filtered.length;
  let html = '';
  if (filtered.length === 0) {
    html = `<div class="terminal-line" style="color: var(--text-muted); font-style: italic; padding: 0.5rem 0;">No log messages recorded for this sensor category yet.</div>`;
  } else {
    filtered.forEach(log => {
      const timeStr = formatMillis(log.ts);
      const catClass = 'badge-' + (log.cat || 'system').toLowerCase();
      const catLabel = (log.cat || 'SYS').toUpperCase();
      const msgHtml = highlightLog(log.msg);
      html += `<div class="terminal-line"><span class="log-time">${timeStr}</span><span class="log-badge ${catClass}">${catLabel}</span><span class="log-msg">${msgHtml}</span></div>`;
    });
  }
  screen.innerHTML = html;
  if (autoScroll) {
    screen.scrollTop = screen.scrollHeight;
  }
}
function updateLogCounts() {
  const counts = { all: allLogs.length, rfid: 0, smoke: 0, climate: 0, light: 0, window: 0, system: 0 };
  allLogs.forEach(l => {
    const c = (l.cat || 'system').toLowerCase();
    if (counts[c] !== undefined) counts[c]++;
    else counts.system++;
  });
  const setC = (id, val) => { const el = document.getElementById(id); if (el) el.innerText = val; };
  setC('countAll', counts.all);
  setC('countRfid', counts.rfid);
  setC('countSmoke', counts.smoke);
  setC('countClimate', counts.climate);
  setC('countLight', counts.light);
  setC('countWindow', counts.window);
  setC('countSystem', counts.system);
}
function setLogCategory(cat) {
  activeLogCategory = cat;
  document.querySelectorAll('.terminal-tab').forEach(b => {
    if (b.getAttribute('data-cat') === cat) b.classList.add('active');
    else b.classList.remove('active');
  });
  const catNames = {
    all: "All Subsystems",
    rfid: "RFID & Door Access",
    smoke: "Smoke & Gas (MQ-2)",
    climate: "Climate (DHT11 & Fan)",
    light: "Lighting (LDR Sensor)",
    window: "Motorized Windows",
    system: "System & Network"
  };
  const activeLabel = document.getElementById('activeCatName');
  if (activeLabel) activeLabel.innerText = catNames[cat] || "Sensor Logs";
  renderLogs();
}
function togglePauseLogs() {
  isLogsPaused = !isLogsPaused;
  const btn = document.getElementById('txtPauseLogs');
  if (isLogsPaused) {
    btn.innerText = '▶ Resume';
    btn.parentElement.style.background = 'rgba(234, 179, 8, 0.2)';
    btn.parentElement.style.color = '#FACC15';
  } else {
    btn.innerText = '⏸ Pause';
    btn.parentElement.style.background = '';
    btn.parentElement.style.color = '';
    renderLogs();
  }
}
async function clearLogsOnServer() {
  try {
    await fetch('/api/logs/clear', { method: 'POST' });
    allLogs = [];
    logSeenSet.clear();
    updateLogCounts();
    renderLogs();
  } catch (e) {
    console.warn("Clear error:", e);
  }
}
async function updateDashboard() {
  try {
    const res = await fetch('/api/status');
    if (!res.ok) return;
    const data = await res.json();
    document.getElementById('valTemp').innerText = data.temperature.toFixed(1);
    document.getElementById('valHum').innerText = data.humidity.toFixed(1);
    document.getElementById('valLight').innerText = data.lightIntensity;
    document.getElementById('valSmoke').innerText = data.smokeLevel;
    let lightDesc = "Normal Daylight";
    if (data.lightIntensity < 1000) lightDesc = "Dark Classroom";
    else if (data.lightIntensity < 2000) lightDesc = "Dim Classroom";
    else if (data.lightIntensity > 3200) lightDesc = "Bright Sunlight";
    document.getElementById('subLight').innerText = lightDesc;
    const banner = document.getElementById('emergencyBanner');
    const smokePill = document.getElementById('pillSmoke');
    if (data.smokeDetected) {
      banner.style.display = 'flex';
      smokePill.className = 'status-pill status-danger';
      smokePill.innerText = 'EMERGENCY ALERT';
    } else {
      banner.style.display = 'none';
      smokePill.className = 'status-pill status-on';
      smokePill.innerText = 'NORMAL';
    }
    const lightPill = document.getElementById('pillLight');
    lightPill.className = 'status-pill ' + (data.lightOn ? 'status-on' : 'status-off');
    lightPill.innerText = data.lightOn ? 'ON' : 'OFF';
    document.querySelectorAll('.btn-light').forEach(b => b.classList.remove('active'));
    if (data.lightMode === 'AUTO') document.getElementById('btnLightAuto').classList.add('active');
    else if (data.lightMode === 'MANUAL_ON') document.getElementById('btnLightOn').classList.add('active');
    else if (data.lightMode === 'MANUAL_OFF') document.getElementById('btnLightOff').classList.add('active');
    const fanPill = document.getElementById('pillFan');
    fanPill.className = 'status-pill ' + (data.fanOn ? 'status-on' : 'status-off');
    fanPill.innerText = data.fanOn ? 'ON' : 'OFF';
    document.querySelectorAll('.btn-fan').forEach(b => b.classList.remove('active'));
    if (data.fanMode === 'AUTO') document.getElementById('btnFanAuto').classList.add('active');
    else if (data.fanMode === 'MANUAL_ON') document.getElementById('btnFanOn').classList.add('active');
    else if (data.fanMode === 'MANUAL_OFF') document.getElementById('btnFanOff').classList.add('active');
    const winPill = document.getElementById('pillWindow');
    if (winPill) {
      winPill.className = 'status-pill ' + (data.windowOpen ? 'status-on' : 'status-off');
      winPill.innerText = data.windowOpen ? 'OPEN' : 'CLOSED';
    }
    const winStateEl = document.getElementById('valWindowState');
    if (winStateEl) winStateEl.innerText = data.windowState || (data.windowOpen ? 'OPEN' : 'CLOSED');
    const doorPill = document.getElementById('pillDoor');
    if (doorPill) {
      if (data.doorState === 'OPENING') {
        doorPill.className = 'status-pill status-warn';
        doorPill.innerText = 'OPENING';
      } else if (data.doorState === 'CLOSING') {
        doorPill.className = 'status-pill status-warn';
        doorPill.innerText = 'CLOSING';
      } else if (data.doorState === 'OPEN' || data.doorOpen || !data.doorLocked) {
        doorPill.className = 'status-pill status-on';
        doorPill.innerText = 'OPEN';
      } else {
        doorPill.className = 'status-pill status-off';
        doorPill.innerText = 'LOCKED';
      }
    }
    const rfidBox = document.getElementById('rfidLiveBox');
    const rfidUidEl = document.getElementById('rfidUidDisplay');
    const rfidNameEl = document.getElementById('rfidNameDisplay');
    const rfidPill = document.getElementById('rfidStatusPill');
    const rfidDescIcon = document.getElementById('rfidDescIcon');
    const rfidDescText = document.getElementById('rfidDescText');
    const rfidTimeAgoEl = document.getElementById('rfidTimeAgo');
    const uid = data.lastRfidUid || 'None';
    const name = data.lastRfidName || 'Unknown Card';
    const status = data.lastRfidStatus || 'IDLE';
    const isAuth = !!data.lastRfidAuthorized;
    const timeAgo = data.lastRfidTimeAgo !== undefined ? data.lastRfidTimeAgo : 999999;
    if (uid && uid !== 'None') {
      rfidUidEl.innerText = uid;
      rfidNameEl.innerText = name;
      if (timeAgo <= 15) {
        if (isAuth) {
          rfidBox.className = 'rfid-live-box granted';
          rfidPill.className = 'status-pill status-on';
          rfidPill.innerText = 'AUTHORIZED';
          rfidDescIcon.innerText = '🟢';
          rfidDescText.innerText = `Access Granted • Door opened (180° -> 0°)`;
          rfidTimeAgoEl.innerText = timeAgo === 0 ? 'Just now' : `${timeAgo}s ago`;
        } else {
          rfidBox.className = 'rfid-live-box denied';
          rfidPill.className = 'status-pill status-danger';
          rfidPill.innerText = 'ACCESS DENIED';
          rfidDescIcon.innerText = '🔴';
          rfidDescText.innerText = `Unauthorized Card • Door remains locked`;
          rfidTimeAgoEl.innerText = timeAgo === 0 ? 'Just now' : `${timeAgo}s ago`;
        }
      } else {
        rfidBox.className = 'rfid-live-box';
        rfidPill.className = 'status-pill status-off';
        rfidPill.innerText = 'STANDBY';
        rfidDescIcon.innerText = '⏱️';
        rfidDescText.innerText = `Last scanned card: ${name}`;
        rfidTimeAgoEl.innerText = timeAgo < 60 ? `${timeAgo}s ago` : `${Math.floor(timeAgo/60)}m ago`;
      }
    } else {
      rfidBox.className = 'rfid-live-box';
      rfidPill.className = 'status-pill status-off';
      rfidPill.innerText = 'IDLE';
      rfidNameEl.innerText = 'Waiting for card...';
      rfidUidEl.innerText = '-- -- -- --';
      rfidDescIcon.innerText = '📡';
      rfidDescText.innerText = 'Present RFID student card to PN532 reader';
      rfidTimeAgoEl.innerText = '';
    }
    const oledHwPill = document.getElementById('pillOledHw');
    if (oledHwPill) {
      if (data.oledAvailable) {
        oledHwPill.className = 'status-pill status-on';
        oledHwPill.innerText = 'I²C READY';
      } else {
        oledHwPill.className = 'status-pill status-warn';
        oledHwPill.innerText = 'I²C 0x3C';
      }
    }
    const lblActiveNotice = document.getElementById('lblActiveNotice');
    if (lblActiveNotice) {
      lblActiveNotice.innerText = (data.notice && data.notice.length > 0) ? data.notice : 'Waiting for IoT message...';
    }
    document.getElementById('badgeIp').innerText = data.ipAddress;
    document.getElementById('badgeSsid').innerText = data.ssid + (data.wifiConnected ? ` (${data.wifiRssi} dBm)` : ' (AP Mode)');
    document.getElementById('badgeUptime').innerText = formatUptime(data.uptimeSeconds);

    const wifiDevs = data.connectedDevices || [];
    const hwDevs = data.hardwareDevices || [];
    cachedWifiDevices = wifiDevs;
    cachedHwDevices = hwDevs;
    const wifiCount = (data.connectedDevicesCount !== undefined) ? data.connectedDevicesCount : wifiDevs.length;
    const hwCount = (data.hardwareDevicesCount !== undefined) ? data.hardwareDevicesCount : hwDevs.length;
    const totalCount = wifiCount + hwCount;

    const badgeDevices = document.getElementById('badgeConnectedCount');
    if (badgeDevices) {
      badgeDevices.innerText = `${wifiCount} Wi-Fi • ${hwCount} HW`;
    }
    const valTotal = document.getElementById('valTotalDevices');
    if (valTotal) valTotal.innerText = totalCount;
    const valWifi = document.getElementById('valWifiDevices');
    if (valWifi) valWifi.innerText = wifiCount;
    const valHw = document.getElementById('valHwDevices');
    if (valHw) valHw.innerText = hwCount;
    const lblSummary = document.getElementById('lblConnectedSummary');
    if (lblSummary) lblSummary.innerText = `${totalCount} Devices Active (${wifiCount} Wi-Fi, ${hwCount} HW)`;
    const countAll = document.getElementById('countPillAll');
    if (countAll) countAll.innerText = totalCount;
    const countWifi = document.getElementById('countPillWifi');
    if (countWifi) countWifi.innerText = wifiCount;
    const countHw = document.getElementById('countPillHw');
    if (countHw) countHw.innerText = hwCount;

    const valNetMode = document.getElementById('valNetMode');
    if (valNetMode) valNetMode.innerText = data.apModeActive ? 'SoftAP Mode' : 'Station Mode';
    const subNetDetail = document.getElementById('subNetDetail');
    if (subNetDetail) subNetDetail.innerText = (data.apModeActive ? 'AP SSID: ' : 'Connected: ') + data.ssid + ` (${data.ipAddress})`;

    renderDevices();
    if (data.students) {
      const statusEl = document.getElementById('attendanceStatus');
      let html = '';
      data.students.forEach(s => {
        const pillClass = s.present ? 'status-on' : 'status-off';
        const pillText = s.present ? 'Present' : 'Absent';
        html += `<div class="attendance-row"><span class="attendance-name">${s.name}</span><span class="status-pill ${pillClass}">${pillText}</span></div>`;
      });
      statusEl.innerHTML = html;
    }
    if (data.attendanceLogs) {
      const logEl = document.getElementById('attendanceLog');
      if (data.attendanceLogs.length === 0) {
        logEl.innerHTML = '<div class="attendance-log-empty">No attendance events recorded yet.</div>';
      } else {
        let html = '';
        for (let i = data.attendanceLogs.length - 1; i >= 0; i--) {
          const e = data.attendanceLogs[i];
          const stateClass = e.present ? 'present' : 'absent';
          const stateText = e.present ? 'Present' : 'Absent';
          const logClass = e.present ? 'log-present' : 'log-absent';
          const totalSec = Math.floor(e.ts / 1000);
          const hh = String(Math.floor(totalSec / 3600)).padStart(2, '0');
          const mm = String(Math.floor((totalSec % 3600) / 60)).padStart(2, '0');
          const ss = String(totalSec % 60).padStart(2, '0');
          const timeStr = hh + ':' + mm + ':' + ss;
          html += `<div class="attendance-log-entry ${logClass}"><div><span class="attendance-log-name">${e.name}</span><span class="attendance-log-state ${stateClass}">${stateText}</span></div><span class="attendance-log-time">${timeStr}</span></div>`;
        }
        logEl.innerHTML = html;
      }
    }
    if (data.recentLogs && Array.isArray(data.recentLogs)) {
      let hasNewLogs = false;
      data.recentLogs.forEach(entry => {
        const key = `${entry.ts}-${entry.cat}-${entry.msg}`;
        if (!logSeenSet.has(key)) {
          logSeenSet.add(key);
          allLogs.push(entry);
          hasNewLogs = true;
        }
      });
      if (allLogs.length > 250) {
        allLogs = allLogs.slice(allLogs.length - 250);
      }
      updateLogCounts();
      if (hasNewLogs && !isLogsPaused) {
        renderLogs();
      }
    }
  } catch (err) {
    console.warn("Polling error:", err);
  }
}
async function sendPost(url, body) {
  try {
    await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body
    });
    updateDashboard();
  } catch (e) {
    console.error("API error:", e);
  }
}
function setLight(mode) {
  sendPost('/api/light', 'mode=' + encodeURIComponent(mode));
}
function setFan(mode) {
  sendPost('/api/fan', 'mode=' + encodeURIComponent(mode));
}
function setWindow(action) {
  sendPost('/api/window', 'action=' + action);
}
function setDoor(action) {
  sendPost('/api/door', 'action=' + action);
}
async function publishNotice() {
  const input = document.getElementById('noticeInput');
  const btn = document.getElementById('btnPublishNotice');
  const msg = input.value.trim();
  if (!msg) return;
  btn.disabled = true;
  btn.innerText = 'Sending to I²C OLED...';
  await sendPost('/api/notice', 'notice=' + encodeURIComponent(msg));
  btn.disabled = false;
  btn.innerText = 'Sent to Hardware OLED!';
  setTimeout(() => {
    btn.innerText = '🚀 Send to Hardware I²C OLED';
  }, 2500);
}
registerSelf();
updateDashboard();
setInterval(updateDashboard, 1500);
)rawliteral";
#endif  
