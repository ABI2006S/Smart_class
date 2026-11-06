#ifndef DASHBOARD_H
#define DASHBOARD_H
#include <Arduino.h>
#include "dashboard_html.h"
#include "dashboard_css.h"
#include "dashboard_js.h"
inline void dashboardInit() {
    Serial.println("[DASHBOARD] S5CSE A Presentation Layer Initialized");
}
inline void dashboardUpdate() {
}
inline String getDashboardHtml() {
    String html = String(DASHBOARD_HTML_BODY);
    html.replace("__INJECT_CSS__", String(DASHBOARD_CSS));
    html.replace("__INJECT_JS__", String(DASHBOARD_JS));
    return html;
}
inline String getCaptivePortalHtml() {
    return String(CAPTIVE_PORTAL_HTML_BODY);
}
#endif  
