#ifndef DASHBOARD_CSS_H
#define DASHBOARD_CSS_H
#include <Arduino.h>
const char DASHBOARD_CSS[] PROGMEM = R"rawliteral(
:root {
  --bg-primary: #090D16;
  --bg-card: rgba(19, 27, 46, 0.82);
  --bg-card-hover: rgba(26, 38, 64, 0.95);
  --border-subtle: rgba(255, 255, 255, 0.08);
  --border-glow: rgba(56, 189, 248, 0.35);
  --text-main: #F1F5F9;
  --text-muted: #94A3B8;
  --accent-cyan: #38BDF8;
  --accent-blue: #3B82F6;
  --accent-indigo: #6366F1;
  --accent-emerald: #10B981;
  --accent-amber: #F59E0B;
  --accent-rose: #EF4444;
  --radius-lg: 16px;
  --radius-md: 12px;
  --radius-sm: 8px;
  --font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  background-color: var(--bg-primary);
  background-image: 
    radial-gradient(at 0% 0%, rgba(59, 130, 246, 0.12) 0px, transparent 50%),
    radial-gradient(at 100% 100%, rgba(99, 102, 241, 0.10) 0px, transparent 50%);
  color: var(--text-main);
  font-family: var(--font-family);
  min-height: 100vh;
  padding: 1.5rem 1rem 3rem;
  line-height: 1.5;
}
.container {
  max-width: 1200px;
  margin: 0 auto;
}
header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 1rem;
  margin-bottom: 2rem;
  padding-bottom: 1.25rem;
  border-bottom: 1px solid var(--border-subtle);
}
.brand-title {
  display: flex;
  align-items: center;
  gap: 0.85rem;
}
.brand-icon {
  width: 46px;
  height: 46px;
  background: linear-gradient(135deg, #2563EB, #7C3AED);
  border-radius: var(--radius-md);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.5rem;
  box-shadow: 0 4px 14px rgba(37, 99, 235, 0.35);
}
h1 {
  font-size: 1.75rem;
  font-weight: 800;
  letter-spacing: -0.02em;
  background: linear-gradient(to right, #FFFFFF, #93C5FD);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}
.subtitle {
  font-size: 0.85rem;
  font-weight: 600;
  color: var(--accent-cyan);
  text-transform: uppercase;
  letter-spacing: 0.12em;
}
.header-meta {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  flex-wrap: wrap;
}
.badge {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  padding: 0.35rem 0.75rem;
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid var(--border-subtle);
  border-radius: 9999px;
  font-size: 0.78rem;
  font-weight: 500;
  color: var(--text-muted);
}
.badge-active {
  background: rgba(16, 185, 129, 0.12);
  border-color: rgba(16, 185, 129, 0.3);
  color: #34D399;
}
.badge-pulse {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background-color: #34D399;
  box-shadow: 0 0 8px #34D399;
  animation: pulse 2s infinite;
}
@keyframes pulse {
  0% { transform: scale(0.95); opacity: 0.8; }
  50% { transform: scale(1.2); opacity: 1; }
  100% { transform: scale(0.95); opacity: 0.8; }
}
#emergencyBanner {
  display: none;
  background: linear-gradient(135deg, rgba(239, 68, 68, 0.95), rgba(185, 28, 28, 0.95));
  border: 2px solid #FCA5A5;
  border-radius: var(--radius-md);
  padding: 1rem 1.25rem;
  margin-bottom: 2rem;
  color: #FFFFFF;
  font-weight: 700;
  align-items: center;
  gap: 1rem;
  box-shadow: 0 0 25px rgba(239, 68, 68, 0.5);
  animation: alarmPulse 1.5s infinite;
}
@keyframes alarmPulse {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.01); }
}
.section-title {
  font-size: 0.95rem;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: var(--text-muted);
  margin-bottom: 1rem;
  display: flex;
  align-items: center;
  gap: 0.5rem;
}
.section-title::after {
  content: "";
  flex: 1;
  height: 1px;
  background: var(--border-subtle);
}
.grid-env {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(230px, 1fr));
  gap: 1.25rem;
  margin-bottom: 2rem;
}
.grid-controls {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 1.25rem;
  margin-bottom: 2rem;
}
.card {
  background: var(--bg-card);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-lg);
  padding: 1.25rem;
  transition: all 0.25s ease;
  position: relative;
  overflow: hidden;
}
.card:hover {
  background: var(--bg-card-hover);
  border-color: rgba(255, 255, 255, 0.16);
  transform: translateY(-2px);
}
.card-top {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.75rem;
}
.card-label {
  font-size: 0.82rem;
  font-weight: 600;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.04em;
}
.card-icon {
  font-size: 1.25rem;
}
.card-value {
  font-size: 2.1rem;
  font-weight: 800;
  letter-spacing: -0.03em;
  color: #FFFFFF;
  margin-bottom: 0.25rem;
}
.card-unit {
  font-size: 1.1rem;
  font-weight: 500;
  color: var(--text-muted);
  margin-left: 0.2rem;
}
.card-subtext {
  font-size: 0.8rem;
  color: var(--text-muted);
}
.status-pill {
  display: inline-block;
  padding: 0.2rem 0.65rem;
  border-radius: 9999px;
  font-size: 0.75rem;
  font-weight: 700;
  text-transform: uppercase;
}
.status-on { background: rgba(16, 185, 129, 0.2); color: #34D399; border: 1px solid rgba(16, 185, 129, 0.3); }
.status-off { background: rgba(148, 163, 184, 0.15); color: #94A3B8; border: 1px solid rgba(148, 163, 184, 0.25); }
.status-warn { background: rgba(245, 158, 11, 0.2); color: #FBBF24; border: 1px solid rgba(245, 158, 11, 0.3); }
.status-danger { background: rgba(239, 68, 68, 0.25); color: #F87171; border: 1px solid rgba(239, 68, 68, 0.4); }
.btn-group {
  display: flex;
  background: rgba(15, 23, 42, 0.65);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-sm);
  padding: 3px;
  gap: 3px;
  margin-top: 1rem;
}
.btn {
  flex: 1;
  padding: 0.6rem 0.5rem;
  font-size: 0.82rem;
  font-weight: 600;
  border: none;
  border-radius: 6px;
  background: transparent;
  color: var(--text-muted);
  cursor: pointer;
  transition: all 0.2s ease;
  text-align: center;
}
.btn:hover {
  color: #FFFFFF;
  background: rgba(255, 255, 255, 0.05);
}
.btn.active {
  background: #2563EB;
  color: #FFFFFF;
  box-shadow: 0 2px 8px rgba(37, 99, 235, 0.4);
}
.btn-emerald.active {
  background: #059669;
  box-shadow: 0 2px 8px rgba(5, 150, 105, 0.4);
}
.btn-danger {
  background: rgba(239, 68, 68, 0.15);
  color: #F87171;
  border: 1px solid rgba(239, 68, 68, 0.3);
}
.btn-danger:hover {
  background: #EF4444;
  color: #FFFFFF;
}
.notice-section {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 1.5rem;
  margin-bottom: 2rem;
}
@media (max-width: 860px) {
  .notice-section { grid-template-columns: 1fr; }
}
.oled-preview {
  background: #000000;
  border: 4px solid #1E293B;
  border-radius: var(--radius-md);
  padding: 1.25rem;
  min-height: 180px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  box-shadow: inset 0 0 15px rgba(56, 189, 248, 0.15), 0 8px 24px rgba(0, 0, 0, 0.6);
  font-family: "Courier New", Courier, monospace;
  position: relative;
}
.oled-preview::after {
  content: "";
  position: absolute;
  top: 0; left: 0; right: 0; bottom: 0;
  background: linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.25) 50%);
  background-size: 100% 3px;
  pointer-events: none;
}
.oled-header {
  font-size: 0.8rem;
  color: #38BDF8;
  border-bottom: 1px dashed #0284C7;
  padding-bottom: 0.35rem;
  display: flex;
  justify-content: space-between;
}
.oled-content {
  color: #E0F2FE;
  font-size: 1.05rem;
  font-weight: 700;
  margin: 1rem 0;
  line-height: 1.4;
  word-break: break-word;
}
.oled-footer {
  font-size: 0.75rem;
  color: #38BDF8;
  border-top: 1px dashed #0284C7;
  padding-top: 0.35rem;
  display: flex;
  justify-content: space-between;
}
.notice-input-box {
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
}
textarea {
  width: 100%;
  height: 110px;
  background: rgba(15, 23, 42, 0.8);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-md);
  padding: 0.85rem;
  color: #FFFFFF;
  font-family: inherit;
  font-size: 0.95rem;
  resize: vertical;
}
textarea:focus {
  outline: none;
  border-color: var(--accent-blue);
  box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.25);
}
.btn-submit {
  padding: 0.8rem 1.25rem;
  background: linear-gradient(135deg, #2563EB, #1D4ED8);
  color: #FFFFFF;
  font-weight: 700;
  font-size: 0.92rem;
  border: none;
  border-radius: var(--radius-md);
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
}
.btn-submit:hover {
  background: linear-gradient(135deg, #3B82F6, #2563EB);
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(37, 99, 235, 0.4);
}
footer {
  margin-top: 3rem;
  padding-top: 1.5rem;
  border-top: 1px solid var(--border-subtle);
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 1rem;
  font-size: 0.82rem;
  color: var(--text-muted);
}
.footer-links a {
  color: var(--accent-cyan);
  text-decoration: none;
  font-weight: 600;
}
.footer-links a:hover {
  text-decoration: underline;
}
.attendance-status {
  display: flex;
  flex-direction: column;
  gap: 0.6rem;
  margin-top: 0.5rem;
}
.attendance-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.65rem 0.85rem;
  background: rgba(15, 23, 42, 0.6);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-sm);
}
.attendance-name {
  font-size: 0.92rem;
  font-weight: 600;
  color: var(--text-main);
}
.attendance-log {
  margin-top: 0.5rem;
  max-height: 260px;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 0.4rem;
  padding-right: 0.25rem;
}
.attendance-log::-webkit-scrollbar {
  width: 5px;
}
.attendance-log::-webkit-scrollbar-track {
  background: transparent;
}
.attendance-log::-webkit-scrollbar-thumb {
  background: rgba(148, 163, 184, 0.3);
  border-radius: 4px;
}
.attendance-log-entry {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.5rem 0.75rem;
  background: rgba(15, 23, 42, 0.5);
  border-left: 3px solid var(--accent-cyan);
  border-radius: 0 var(--radius-sm) var(--radius-sm) 0;
  font-size: 0.84rem;
}
.attendance-log-entry.log-present {
  border-left-color: var(--accent-emerald);
}
.attendance-log-entry.log-absent {
  border-left-color: var(--accent-amber);
}
.attendance-log-name {
  font-weight: 600;
  color: var(--text-main);
}
.attendance-log-state {
  font-weight: 600;
  margin-left: 0.5rem;
}
.attendance-log-state.present { color: #34D399; }
.attendance-log-state.absent  { color: #FBBF24; }
.attendance-log-time {
  font-size: 0.78rem;
  color: var(--text-muted);
  font-family: "Courier New", Courier, monospace;
}
.attendance-log-empty {
  font-size: 0.85rem;
  color: var(--text-muted);
  text-align: center;
  padding: 1.5rem 0;
  font-style: italic;
}
.rfid-live-box {
  margin: 0.75rem 0;
  padding: 0.85rem;
  border-radius: var(--radius-md);
  background: rgba(15, 23, 42, 0.65);
  border: 1px solid var(--border-subtle);
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
}
.rfid-live-box.granted {
  background: linear-gradient(135deg, rgba(16, 185, 129, 0.18), rgba(6, 78, 59, 0.25));
  border-color: rgba(16, 185, 129, 0.6);
  box-shadow: 0 0 16px rgba(16, 185, 129, 0.2);
}
.rfid-live-box.denied {
  background: linear-gradient(135deg, rgba(239, 68, 68, 0.2), rgba(127, 29, 29, 0.25));
  border-color: rgba(239, 68, 68, 0.65);
  box-shadow: 0 0 16px rgba(239, 68, 68, 0.25);
  animation: pulseDenied 1.5s infinite;
}
@keyframes pulseDenied {
  0% { box-shadow: 0 0 8px rgba(239, 68, 68, 0.2); }
  50% { box-shadow: 0 0 18px rgba(239, 68, 68, 0.45); }
  100% { box-shadow: 0 0 8px rgba(239, 68, 68, 0.2); }
}
.rfid-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.5rem;
}
.rfid-title {
  font-size: 0.78rem;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: var(--text-muted);
  display: flex;
  align-items: center;
  gap: 0.4rem;
}
.rfid-uid-chip {
  font-family: "JetBrains Mono", Consolas, "Courier New", monospace;
  font-size: 0.92rem;
  font-weight: 700;
  padding: 0.2rem 0.5rem;
  border-radius: var(--radius-sm);
  background: rgba(0, 0, 0, 0.35);
  border: 1px solid rgba(255, 255, 255, 0.1);
  color: var(--accent-cyan);
  letter-spacing: 0.06em;
}
.rfid-name-row {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 0.5rem;
  margin-top: 0.35rem;
}
.rfid-name {
  font-size: 1.15rem;
  font-weight: 800;
  color: #FFFFFF;
}
.rfid-timeago {
  font-size: 0.75rem;
  color: var(--text-muted);
}
.rfid-desc {
  font-size: 0.8rem;
  margin-top: 0.25rem;
  color: #94A3B8;
  display: flex;
  align-items: center;
  gap: 0.35rem;
}
.terminal-card {
  padding: 1.25rem;
  background: rgba(13, 19, 33, 0.92);
  border: 1px solid rgba(56, 189, 248, 0.25);
  border-radius: var(--radius-lg);
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
  margin-top: 1rem;
}
.terminal-tabs-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 0.75rem;
  margin-bottom: 0.85rem;
  padding-bottom: 0.85rem;
  border-bottom: 1px solid var(--border-subtle);
}
.terminal-tabs {
  display: flex;
  flex-wrap: wrap;
  gap: 0.4rem;
}
.terminal-tab {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  padding: 0.4rem 0.75rem;
  font-size: 0.82rem;
  font-weight: 600;
  border-radius: var(--radius-sm);
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid rgba(255, 255, 255, 0.08);
  color: var(--text-muted);
  cursor: pointer;
  transition: all 0.2s ease;
}
.terminal-tab:hover {
  background: rgba(255, 255, 255, 0.08);
  color: var(--text-main);
  border-color: rgba(255, 255, 255, 0.2);
}
.terminal-tab.active {
  background: rgba(56, 189, 248, 0.15);
  border-color: var(--accent-cyan);
  color: #FFFFFF;
  box-shadow: 0 0 12px rgba(56, 189, 248, 0.25);
}
.tab-count {
  font-size: 0.72rem;
  padding: 0.1rem 0.4rem;
  border-radius: 999px;
  background: rgba(0, 0, 0, 0.35);
  color: var(--text-muted);
}
.terminal-tab.active .tab-count {
  background: var(--accent-cyan);
  color: #0F172A;
  font-weight: 700;
}
.terminal-controls {
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  gap: 0.6rem;
}
.search-box {
  display: flex;
  align-items: center;
  gap: 0.35rem;
  background: rgba(0, 0, 0, 0.35);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-sm);
  padding: 0.25rem 0.6rem;
}
.search-box input {
  background: transparent;
  border: none;
  outline: none;
  color: var(--text-main);
  font-size: 0.82rem;
  width: 140px;
}
.search-box input::placeholder {
  color: var(--text-muted);
}
.toggle-autoscroll {
  display: flex;
  align-items: center;
  gap: 0.35rem;
  font-size: 0.8rem;
  color: var(--text-muted);
  cursor: pointer;
  user-select: none;
}
.toggle-autoscroll input {
  cursor: pointer;
  accent-color: var(--accent-cyan);
}
.btn-console {
  padding: 0.35rem 0.65rem;
  font-size: 0.78rem;
  font-weight: 600;
  border-radius: var(--radius-sm);
  background: rgba(255, 255, 255, 0.06);
  border: 1px solid var(--border-subtle);
  color: var(--text-main);
  cursor: pointer;
  transition: all 0.2s ease;
}
.btn-console:hover {
  background: rgba(255, 255, 255, 0.12);
  border-color: rgba(255, 255, 255, 0.2);
}
.terminal-screen {
  background: #070B14;
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: var(--radius-md);
  padding: 0.85rem;
  height: 320px;
  overflow-y: auto;
  font-family: "JetBrains Mono", Consolas, "Courier New", monospace;
  font-size: 0.82rem;
  line-height: 1.6;
  color: #E2E8F0;
  display: flex;
  flex-direction: column;
  gap: 0.25rem;
  box-shadow: inset 0 2px 10px rgba(0, 0, 0, 0.6);
}
.terminal-screen::-webkit-scrollbar {
  width: 6px;
}
.terminal-screen::-webkit-scrollbar-track {
  background: #070B14;
}
.terminal-screen::-webkit-scrollbar-thumb {
  background: rgba(148, 163, 184, 0.25);
  border-radius: 4px;
}
.terminal-line {
  display: flex;
  align-items: baseline;
  gap: 0.5rem;
  word-break: break-word;
  padding: 0.1rem 0;
  transition: background 0.15s ease;
}
.terminal-line:hover {
  background: rgba(255, 255, 255, 0.03);
}
.log-time {
  color: #64748B;
  font-size: 0.76rem;
  flex-shrink: 0;
  user-select: none;
}
.log-badge {
  font-size: 0.68rem;
  font-weight: 700;
  padding: 0.1rem 0.4rem;
  border-radius: 4px;
  text-transform: uppercase;
  letter-spacing: 0.04em;
  flex-shrink: 0;
  user-select: none;
}
.badge-rfid    { background: rgba(16, 185, 129, 0.2); color: #34D399; border: 1px solid rgba(16, 185, 129, 0.35); }
.badge-smoke   { background: rgba(244, 63, 94, 0.2);  color: #FB7185; border: 1px solid rgba(244, 63, 94, 0.35); }
.badge-climate { background: rgba(14, 165, 233, 0.2); color: #38BDF8; border: 1px solid rgba(14, 165, 233, 0.35); }
.badge-light   { background: rgba(234, 179, 8, 0.2);  color: #FACC15; border: 1px solid rgba(234, 179, 8, 0.35); }
.badge-window  { background: rgba(168, 85, 247, 0.2); color: #C084FC; border: 1px solid rgba(168, 85, 247, 0.35); }
.badge-system  { background: rgba(59, 130, 246, 0.2); color: #60A5FA; border: 1px solid rgba(59, 130, 246, 0.35); }
.log-msg {
  color: #CBD5E1;
  flex-grow: 1;
}
.log-msg .kw-good { color: #34D399; font-weight: 600; }
.log-msg .kw-bad  { color: #F87171; font-weight: 600; }
.log-msg .kw-num  { color: #38BDF8; }
.terminal-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 0.5rem;
  margin-top: 0.65rem;
  font-size: 0.78rem;
  color: var(--text-muted);
}
.device-filter-pills {
  display: flex;
  align-items: center;
  gap: 0.4rem;
  flex-wrap: wrap;
}
.filter-pill {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid var(--border-subtle);
  color: var(--text-muted);
  padding: 0.35rem 0.75rem;
  border-radius: 9999px;
  font-size: 0.78rem;
  cursor: pointer;
  transition: all 0.2s ease;
  font-family: inherit;
}
.filter-pill:hover {
  background: rgba(255, 255, 255, 0.1);
  color: var(--text-main);
}
.filter-pill.active {
  background: rgba(56, 189, 248, 0.18);
  border-color: rgba(56, 189, 248, 0.45);
  color: var(--accent-cyan);
  font-weight: 600;
}
.devices-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 1rem;
}
.device-item-card {
  background: var(--bg-card);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-md);
  padding: 1rem;
  display: flex;
  flex-direction: column;
  gap: 0.65rem;
  transition: transform 0.15s ease, border-color 0.15s ease, box-shadow 0.15s ease;
}
.device-item-card:hover {
  transform: translateY(-2px);
  border-color: var(--border-glow);
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.35);
}
.device-card-head {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 0.5rem;
}
.device-title-area {
  display: flex;
  align-items: center;
  gap: 0.65rem;
}
.device-icon-box {
  width: 40px;
  height: 40px;
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.35rem;
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.08);
  flex-shrink: 0;
}
.device-name-text {
  font-weight: 700;
  font-size: 0.95rem;
  color: var(--text-main);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  max-width: 155px;
}
.device-cat-badge {
  font-size: 0.72rem;
  color: var(--accent-cyan);
  text-transform: uppercase;
  letter-spacing: 0.05em;
  font-weight: 600;
}
.device-meta-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 0.78rem;
  color: var(--text-muted);
  padding-top: 0.4rem;
  border-top: 1px solid rgba(255, 255, 255, 0.05);
}
.device-meta-val {
  font-family: monospace;
  color: #CBD5E1;
}
.device-btn-rename {
  background: rgba(56, 189, 248, 0.12);
  border: 1px solid rgba(56, 189, 248, 0.3);
  color: var(--accent-cyan);
  border-radius: 6px;
  padding: 0.25rem 0.55rem;
  font-size: 0.72rem;
  cursor: pointer;
  font-weight: 600;
  transition: all 0.15s ease;
  font-family: inherit;
}
.device-btn-rename:hover {
  background: rgba(56, 189, 248, 0.25);
  color: #FFFFFF;
}
.modal-overlay {
  position: fixed;
  inset: 0;
  background: rgba(0, 0, 0, 0.75);
  backdrop-filter: blur(5px);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 9999;
  padding: 1rem;
}
.modal-card {
  background: #111827;
  border: 1px solid rgba(255, 255, 255, 0.14);
  border-radius: 16px;
  padding: 1.5rem;
  max-width: 420px;
  width: 100%;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.7);
}
.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
}
.modal-close-btn {
  background: none;
  border: none;
  color: var(--text-muted);
  font-size: 1.5rem;
  cursor: pointer;
  padding: 0 0.5rem;
}
.modal-close-btn:hover {
  color: var(--text-main);
}
.modal-input {
  width: 100%;
  background: rgba(0, 0, 0, 0.35);
  border: 1px solid rgba(255, 255, 255, 0.12);
  border-radius: 8px;
  padding: 0.65rem 0.85rem;
  color: #F1F5F9;
  font-size: 0.9rem;
  outline: none;
  font-family: inherit;
  box-sizing: border-box;
}
.modal-input:focus {
  border-color: var(--accent-cyan);
  box-shadow: 0 0 0 2px rgba(56, 189, 248, 0.25);
}
)rawliteral";
#endif  
