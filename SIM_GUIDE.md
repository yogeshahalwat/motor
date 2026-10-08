# SIM Card Guide for IoT Motor Controller

## Two Types of SIMs — Which One Do We Need?

### Type 1: Regular Consumer SIM (what's in your phone)

A normal prepaid/postpaid SIM you buy from a Jio/Airtel store. Designed for humans making calls, sending messages, and browsing the internet.

| Aspect | Details |
|--------|---------|
| **Where to buy** | Any mobile shop, online (Jio.com, Airtel.in) |
| **KYC** | Individual Aadhaar-based |
| **Number format** | 10-digit mobile number |
| **Temperature rating** | -25C to +85C |
| **Lifespan** | 1-3 years |
| **Plans** | Voice + SMS + Data bundles |
| **Management** | Manual recharge, individual account |
| **Can it work in an IoT device?** | **Yes, technically works.** Many hobby and small-scale projects use regular SIMs in SIM800/SIM7600 modules. |

### Type 2: M2M (Machine-to-Machine) SIM — what companies use

A specialized SIM designed specifically for devices (machines talking to machines). This is what commercial products like GPS trackers, smart meters, POS machines, and products like KisanRaja use.

| Aspect | Details |
|--------|---------|
| **Where to buy** | Enterprise sales only — contact Jio Business, Airtel IoT, BSNL, Vi |
| **KYC** | Enterprise/bulk KYC (company registration) |
| **Number format** | 13-digit M2M number (DoT India regulation) |
| **Temperature rating** | -40C to +105C (important for outdoor motor boxes) |
| **Lifespan** | 10-15 years |
| **Plans** | Data-only, very low cost (Rs 15-50/month) |
| **Management** | Fleet dashboard — monitor all SIMs remotely, check data usage, activate/deactivate |
| **Form factors** | Standard/Micro/Nano + MFF2 (chip soldered directly to PCB, cannot be removed) |
| **Vibration/humidity** | Rated for 95% humidity, 20G vibration |

### Why M2M SIMs Exist

Regular SIMs are designed for a phone in your pocket (room temperature, dry, replaced every 1-2 years when you upgrade phone). A motor controller sits outdoors in a metal box, exposed to heat/cold/humidity/vibration, running 24/7 for years. M2M SIMs are physically tougher and have plans designed for tiny data usage — you don't pay for voice minutes and SMS you'll never use.

---

## Indian Telecom Providers for IoT

### Jio IoT (via JioThings)

| Aspect | Details |
|--------|---------|
| **Brand** | JioThings (jiothings.com) |
| **Network** | 4G LTE, NB-IoT (Narrowband IoT) |
| **NO 2G support** | Jio never built a 2G network. SIM800L (2G module) will NOT work with Jio |
| **Pricing** | Not publicly listed. Enterprise sales only |
| **Estimated cost** | Rs 15-50/month for low-data M2M plans (based on industry sources) |
| **Coverage** | Widest 4G coverage in rural India |
| **How to get** | Contact Jio Business at jio.com/business |
| **Minimum order** | Likely requires bulk order (10-100+ SIMs) |
| **Used by** | Smart meters, fleet tracking (Ola/Uber), JioTag devices |
| **NB-IoT** | Available — ideal for ultra-low-data devices. Requires NB-IoT compatible module (SIM7020E, BC66), not standard SIM7600 |

### Airtel IoT

| Aspect | Details |
|--------|---------|
| **Brand** | Airtel IoT (airtel.in/business/b2b/iot) |
| **Network** | 5G, 4G, NB-IoT, LTE-M, 2G, Satellite |
| **2G support** | Yes — SIM800L works on Airtel (for now) |
| **Pricing** | Not publicly listed. Enterprise sales only |
| **Estimated cost** | Rs 20-50/month for basic M2M data plans |
| **Scale** | 45 million connected devices, 11,000+ enterprise customers |
| **SIM types** | Physical SIM, eSIM, dual-profile eSIMs |
| **SIM durability** | -40C to +105C, 95% humidity, 20G vibration, 10-15 year lifespan |
| **How to get** | Click "Talk to an Expert" at airtel.in/business/b2b/iot |
| **Products** | IoTHub (device management), IoT Locate (tracking), IoT Super Tracker |
| **Used by** | ATMs, POS machines, fleet tracking, smart agriculture |

### BSNL

| Aspect | Details |
|--------|---------|
| **Network** | 2G, 3G, 4G (coverage varies by region) |
| **2G support** | Yes — widest 2G rural coverage |
| **Pricing** | Historically cheapest operator |
| **Estimated cost** | Rs 30-99/month for IoT data plans |
| **Coverage** | Best rural coverage overall (especially in remote areas) |
| **How to get** | Contact BSNL enterprise division |
| **Advantage** | Government operator, unlikely to shut down 2G quickly |
| **Disadvantage** | Slower service, less reliable network in some areas |

### Vi (Vodafone Idea)

| Aspect | Details |
|--------|---------|
| **Network** | 4G, 2G |
| **2G support** | Yes |
| **Pricing** | Not publicly listed |
| **Estimated cost** | Rs 25-75/month |
| **How to get** | Contact 7292000444 |
| **Products** | IoT eSIMs, Smart Infrastructure, IoT Smart Central platform |
| **Risk** | Vi has financial challenges — long-term viability uncertain for multi-year IoT deployments |

### Global IoT SIM Providers (Work in India via Roaming)

| Provider | India Coverage | Pricing | Best For |
|----------|--------------|---------|----------|
| **Hologram** | Yes (via roaming partners) | $1/month + $0.03/MB (~Rs 84/month + Rs 2.5/MB) | Prototyping, global projects |
| **1NCE** | Unconfirmed for India | $14 one-time for 10 years (~Rs 1,170 total) | Cheapest long-term IF India is covered |
| **Twilio Super SIM** | Yes | Pay-per-use | Developer-friendly API |
| **Soracom** | Limited | Pay-per-use | Japanese provider |
| **emnify** | Yes (via roaming) | Contact sales | German provider, European focus |
| **Sensorise** | Yes (Indian native) | Contact sales | Indian M2M specialist |

---

## Can We Use a Regular Consumer SIM? Legal and Practical Concerns

### Legally

| Situation | Status |
|-----------|--------|
| **1-2 devices for personal/prototype use** | Fine. No one enforces M2M regulations at this scale |
| **Selling devices commercially** | Must use proper M2M SIMs with 13-digit numbering per DoT guidelines |
| **DoT M2M Guidelines** | All M2M Service Providers must register. As of September 2024, enforcement tightened |
| **TRAI Recommendation** | Separate M2M numbering and licensing framework recommended |

### Practically — Issues with Consumer SIMs in IoT Devices

| Issue | Risk Level | Details |
|-------|-----------|---------|
| **Auto-deactivation** | Medium | Operators may flag and deactivate SIMs that never make voice calls as "inactive" |
| **Manual recharge** | High (long-term) | Consumer prepaid plans expire. If you forget to recharge, the motor controller goes offline. M2M plans auto-renew |
| **Temperature** | Medium | Consumer SIMs rated to +85C. Metal box in direct sunlight in Indian summer can exceed this |
| **KYC scaling** | High (at scale) | Each consumer SIM needs individual Aadhaar KYC. Cannot register 50 SIMs easily |
| **No fleet management** | Medium | Cannot remotely monitor data usage, activate/deactivate, or manage from a dashboard |
| **Plan waste** | Low | You pay for voice/SMS bundles you don't use |
| **Lifespan** | Low (short-term) | Consumer SIM may need replacement in 2-3 years vs 10-15 years for M2M |

### Verdict

- **For development and POC (1-5 devices)**: Use regular consumer SIM. It works, it's cheap, it's instant.
- **For production deployment (10+ devices)**: Switch to proper M2M SIMs from Jio or Airtel.

---

## How Much Data Does Our Motor Controller Need?

### What Our Device Sends/Receives

| Operation | Payload Size | Frequency | Direction |
|-----------|-------------|-----------|-----------|
| Read command from Firebase (check if farmer sent ON/OFF) | ~500 bytes response | Every 5-30 seconds | Download |
| Write status to Firebase (motor state, fault, etc.) | ~300 bytes request | Every 1-5 minutes | Upload |
| Push notification trigger (fault/motor off events) | ~200 bytes | On event only (rare) | Upload |
| SSL/TLS handshake (HTTPS overhead) | ~5-10 KB | Once per connection | Both |

### Actual Protocol Overhead Per HTTP Request

The raw payload (e.g., 500 bytes of JSON) is NOT the total data sent. There's protocol overhead:

| Layer | Overhead Per Request |
|-------|---------------------|
| HTTP headers (request) | 200-400 bytes |
| HTTP headers (response) | 200-400 bytes |
| TCP/IP headers | ~40 bytes |
| TLS/HTTPS overhead (after handshake) | ~100-200 bytes |
| **Total per transaction** | **~1.5-2 KB** (even for a 500-byte payload) |

### Monthly Data Usage by Polling Interval

Assuming ~2 KB per transaction (read command from Firebase):

| Polling Interval | Requests/Day | Data/Day | Data/Month | Suitable? |
|-----------------|-------------|----------|-----------|-----------|
| Every 5 seconds | 17,280 | 33.75 MB | **~1 GB** | Too much. Expensive and unnecessary for motor control |
| Every 10 seconds | 8,640 | 16.9 MB | **~500 MB** | Still too much |
| Every 30 seconds | 2,880 | 5.6 MB | **~170 MB** | Acceptable but can optimize further |
| **Every 60 seconds** | 1,440 | 2.8 MB | **~85 MB** | Good balance of responsiveness and data usage |
| **Every 5 minutes** | 288 | 0.56 MB | **~17 MB** | Most efficient. Recommended for status updates |

### Recommended Design: Hybrid Approach

Instead of polling at a fixed interval for everything, use a smart approach:

| Operation | Interval | Reason |
|-----------|----------|--------|
| **Check for new command** (did farmer tap ON/OFF?) | Every 15-30 seconds | Farmer expects response within 30 seconds of tapping button |
| **Send status update** (motor state, fault) | Every 5 minutes | Status doesn't change often. No need to send same data repeatedly |
| **Send fault/event alert** | Immediately on detection | Critical events should be instant |
| **Keep-alive / heartbeat** | Every 5 minutes | So the app knows the device is still online |

### With This Design: Monthly Data Estimate

```
Command checks:     2,880/day x 2 KB  = 5.6 MB/day  = 170 MB/month
Status updates:     288/day   x 1 KB  = 0.28 MB/day  = 8.5 MB/month
Event alerts:       ~5/day    x 1 KB  = 0.005 MB/day = 0.15 MB/month
Heartbeats:         288/day   x 0.5KB = 0.14 MB/day  = 4.3 MB/month
TLS handshakes:     ~50/day   x 5 KB  = 0.25 MB/day  = 7.5 MB/month
                                         ----------------------------
TOTAL:                                   ~190 MB/month
```

### Further Optimization: Use Firebase Realtime Database Streaming

Instead of polling (asking "any new command?" every 30 seconds), Firebase supports **persistent connections** — the device opens one connection and Firebase pushes data to it instantly when something changes. This means:

- No repeated HTTP requests for command checking
- Command arrives within 1-2 seconds of farmer tapping the button
- Data usage drops dramatically (only active when data changes)

With streaming:
```
Persistent connection overhead: ~50 MB/month (keep-alive packets)
Status updates (every 5 min):  ~8.5 MB/month
Event alerts:                  ~0.15 MB/month
                                ----------------------
TOTAL WITH STREAMING:           ~60 MB/month
```

**This is the recommended approach.** ~60 MB/month is easily covered by any plan.

---

## SIM Plan Comparison for Our Use Case (~60-200 MB/month)

### Consumer Prepaid Plans (For Development / POC)

| Provider | Plan | Data | Validity | Cost/Month | Our Verdict |
|----------|------|------|----------|-----------|-------------|
| **Jio** | Rs 149 | 2GB/day (total 48GB) | 24 days | Rs 186/month (adjusted to 30-day) | Massive overkill but cheapest. **Best for development with SIM7600 (4G)** |
| **Jio** | Rs 199 | 2GB/day (total 56GB) | 28 days | Rs 213/month | Same, slightly better validity |
| **Airtel** | Rs 149 | 2GB/day | 24 days | Rs 186/month | Same as Jio. **Best for development with SIM800L (2G)** since Airtel has 2G |
| **Airtel** | Rs 179 | 2GB/day | 28 days | Rs 192/month | Better validity |
| **BSNL** | Rs 107 | 1GB/day | 22 days | Rs 146/month | Cheapest but network less reliable |
| **Vi** | Rs 149 | 2GB/day | 24 days | Rs 186/month | Same as others |

**Note**: All consumer plans give WAY more data than we need (we use ~60-200 MB, plans give 48-56 GB). But there's no cheaper consumer plan tier — these are the minimum recharge amounts.

### M2M Plans (For Production Deployment)

| Provider | Estimated Cost/Month | Data Included | How to Get |
|----------|---------------------|--------------|------------|
| **Jio M2M** | Rs 15-50 | Low-data pool (exact MB varies by negotiation) | jio.com/business → Contact sales |
| **Airtel M2M** | Rs 20-50 | Low-data pool | airtel.in/business/b2b/iot → Talk to Expert |
| **BSNL M2M** | Rs 30-99 | Low-data pool | Contact BSNL enterprise |
| **Vi M2M** | Rs 25-75 | Low-data pool | Call 7292000444 |

**Note**: M2M plan pricing is not publicly listed by any Indian operator. These are estimates from industry sources. Actual pricing depends on negotiation, volume, and contract terms.

### Global IoT SIM Providers

| Provider | Monthly Cost for ~200 MB | Total Cost/Month | Notes |
|----------|------------------------|-----------------|-------|
| **Hologram** | $1 base + $0.03 x 200 = $7 | ~Rs 670/month | Too expensive for India |
| **1NCE** | Rs 1,170 one-time / 10 years = ~Rs 10/month amortized | ~Rs 10/month | Cheapest IF India coverage confirmed. Verify before buying |

---

## What Commercial IoT Products in India Use

| Product Category | Typical SIM Provider | SIM Type | Notes |
|-----------------|---------------------|----------|-------|
| **KisanRaja** (farm motor control) | Jio / Airtel | M2M | Bulk enterprise plans |
| **GPS Trackers** (Letstrack, MapmyIndia) | Airtel / Jio | M2M | 50-100 MB/month plans |
| **Smart Electricity Meters** | Jio (NB-IoT) | M2M (NB-IoT) | Ultra-low data |
| **POS Machines** (Paytm, PhonePe) | Airtel / Jio | M2M | Pooled data plans |
| **Ride-hailing** (Ola, Uber driver devices) | Jio | M2M | Bulk procurement |
| **Fleet Tracking** (Rivigo, Blackbuck) | Airtel IoT | M2M | With IoTHub management |
| **ATMs** | BSNL / Airtel | M2M | BSNL popular due to rural coverage |
| **Vending Machines** | Vi / Airtel | M2M | Low-data plans |

**Key takeaway**: Every serious commercial IoT product in India uses M2M SIMs. But all of them started with regular SIMs during development.

---

## 2G Sunset Risk — Important for Module Choice

| Provider | 2G Status | Impact on SIM800L |
|----------|-----------|-------------------|
| **Jio** | Never had 2G. 4G/5G only | **SIM800L will NOT work with Jio** |
| **Airtel** | 2G still active but sunset being discussed (estimated 2027-2028) | Works now. Risk in 2-3 years |
| **Vi** | 2G still active | Works now. Similar sunset risk |
| **BSNL** | 2G still active, likely last to shut down | Safest for 2G, but network quality varies |

### Recommendation Based on Module Choice

| If Your GSM Module Is... | Best SIM Provider | Why |
|--------------------------|-------------------|-----|
| **SIM800L (2G only)** | Airtel or BSNL (consumer SIM for dev, M2M for production) | Only providers with 2G. Not Jio. Short-term only. |
| **SIM7600E (4G)** | Jio (best 4G rural coverage) or Airtel | Both work. Jio has better rural 4G. Future-proof. |
| **A7672S (4G)** | Jio or Airtel | Same as SIM7600 |

---

## Our Decision & Conclusion

### For Development Phase (Starting Now)

| Decision | Choice | Cost | Action |
|----------|--------|------|--------|
| **SIM type** | Regular consumer prepaid SIM | Rs 149-199 | Buy from any store |
| **Provider** | **Jio** (if using SIM7600/A7672S) or **Airtel** (if using SIM800L) | Same | Buy the Rs 149 plan |
| **APN setting in code** | Jio: `"jionet"` / Airtel: `"airtelgprs.com"` | -- | Configure in firmware |

### For Production Phase (10+ Devices)

| Decision | Choice | Cost | Action |
|----------|--------|------|--------|
| **SIM type** | M2M SIM | Rs 15-50/month per SIM | Contact provider's enterprise sales |
| **Provider** | Jio (best rural 4G) or Airtel (if need 2G fallback) | Negotiable | Request IoT/M2M pilot plan |
| **Management** | Fleet dashboard from provider | Usually included | Monitor all SIMs remotely |

### Data Optimization Strategy

| Strategy | Implementation | Data Saved |
|----------|---------------|------------|
| Use Firebase streaming instead of polling | Persistent connection, push-based | Reduces from ~200 MB to ~60 MB/month |
| Send status only when changed | Don't send same "motor ON" every 5 minutes if nothing changed | Reduces by ~50% |
| Batch heartbeats with status | Combine keep-alive and status into one message | Minor savings |
| Use binary payload instead of JSON | Send `0x01` instead of `{"relay":"ON"}` | Reduces payload by ~80% |

### Total Recurring Cost Per Device

| Phase | SIM Cost/Month | Firebase Cost/Month | Total/Month |
|-------|---------------|-------------------|-------------|
| **Development** | Rs 149-199 (consumer SIM) | Rs 0 (free tier) | **Rs 149-199** |
| **Production** | Rs 15-50 (M2M SIM) | Rs 0 (free tier, up to 50 devices) | **Rs 15-50** |

---

## Quick Reference: APN Settings for Firmware Code

```c
// For Jio 4G (SIM7600 / A7672S module)
char apn[] = "jionet";
char user[] = "";
char pass[] = "";

// For Airtel (SIM800L 2G or SIM7600 4G)
char apn[] = "airtelgprs.com";
char user[] = "";
char pass[] = "";

// For BSNL
char apn[] = "bsnlnet";
char user[] = "";
char pass[] = "";

// For Vi (Vodafone Idea)
char apn[] = "vi-internet";  // or "www" on older plans
char user[] = "";
char pass[] = "";
```

---

## Action Items

1. **Now**: Buy a regular Jio or Airtel prepaid SIM (Rs 149) for development
2. **Ask hardware friend**: Which GSM module is on the custom PCB? This determines Jio (4G) vs Airtel (2G/4G)
3. **During development**: Optimize data usage (streaming, event-based updates)
4. **Before production**: Contact Jio Business or Airtel IoT for M2M SIM pilot plan
5. **For production hardware**: Consider MFF2 (embedded chip) SIM form factor if designing next PCB revision — cannot be stolen or accidentally removed

---

*Document created: 2026-10-08*
*Research covers Indian telecom landscape as of October 2026*
