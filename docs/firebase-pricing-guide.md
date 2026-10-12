# Motor Controller — Firebase Cloud Costs & Pricing Guide

---

## What is Firebase?

Firebase is a cloud service by Google. It acts as a **shared database on the internet** that both the mobile app and the motor controller board can read/write to.

- The **farmer's phone app** writes "ON" or "OFF" to Firebase
- The **board in the field** reads that command from Firebase every 5 seconds and controls the motor
- The **board** also writes back motor status (running, fault, all-ok) to Firebase
- The **app** shows that status to the farmer in real-time

Firebase sits in the middle — the app and the board never talk to each other directly.

---

## How the System Uses Data

### Every 5 seconds, each board does:

| Action | What happens | Data used |
|--------|-------------|-----------|
| READ command | Board asks Firebase: "Is motor ON or OFF?" | ~50 bytes downloaded |
| WRITE status | Board tells Firebase: "Motor is running, no fault" | ~100 bytes uploaded |

**Only downloads count toward Firebase billing. Uploads are free.**

### Per farmer per month:

```
50 bytes × 12 times/minute × 60 min × 24 hours × 30 days
= 25 MB downloaded per month per farmer
```

---

## Firebase Free Plan (Spark)

| Resource | Free Limit | What it means |
|----------|-----------|---------------|
| Download bandwidth | 10 GB / month | How much data boards + apps can read per month |
| Storage | 1 GB | Total data stored in the database |
| Simultaneous connections | 100 | How many apps can be open at the exact same moment |

### How many farmers fit in the free plan?

```
Free download limit = 10 GB = 10,000 MB
Each farmer uses    = 25 MB / month

10,000 ÷ 25 = 400 farmers — completely FREE
```

### What about storage?

Each farmer's data in the database = ~200 bytes

```
1 GB free storage = 1,000,000,000 bytes
1,000,000,000 ÷ 200 = 5,000,000 (50 lakh) farmers

Storage will NEVER be a problem.
```

### What about the 100 connection limit?

- This only applies to farmers who have the **app open on their phone right now**
- A farmer opens the app for 1-2 minutes, turns motor on/off, closes the app
- In practice, maybe 20-30 farmers out of 500 have the app open at the same time
- So 100 connections can support **300-500 total farmers**

---

## Paid Plan (Blaze — Pay As You Go)

When you cross 400 farmers, upgrade to the Blaze plan. The first 10 GB is still free.

| What | Cost |
|------|------|
| First 10 GB download / month | ₹0 (free) |
| Every extra 1 GB download | ₹83 ($1) |
| Storage (1 GB free, then ₹415/GB) | ₹0 for us (we use almost nothing) |
| Simultaneous connections | Up to 2,00,000 (no practical limit) |

### Cost per farmer

```
1 GB = 40 farmers
₹83 per GB

Cost per farmer = ₹83 ÷ 40 = ₹2 per farmer per month
```

---

## Pricing Table

| Number of Farmers | Monthly Download | Monthly Cost |
|-------------------|-----------------|-------------|
| 50 | 1.25 GB | **₹0** |
| 100 | 2.5 GB | **₹0** |
| 200 | 5 GB | **₹0** |
| 400 | 10 GB | **₹0** |
| 500 | 12.5 GB | **₹250** |
| 1,000 | 25 GB | **₹1,300** |
| 2,000 | 50 GB | **₹3,300** |
| 5,000 | 125 GB | **₹9,600** |
| 10,000 | 250 GB | **₹20,000** |

*Note: Costs can be reduced by polling every 10 or 15 seconds instead of 5 — this cuts bandwidth by 50-67%.*

---

## Cost Optimization Options

| Change | Effect |
|--------|--------|
| Poll every 10 seconds (instead of 5) | Cost reduced by **50%** |
| Poll every 15 seconds (instead of 5) | Cost reduced by **67%** |
| Poll every 30 seconds | Cost reduced by **83%** |

For example, 5,000 farmers at 15-second polling:
```
25 MB ÷ 3 = ~8.3 MB per farmer
5,000 × 8.3 MB = 41.5 GB
Extra = 41.5 - 10 = 31.5 GB
Cost = 31.5 × ₹83 = ₹2,600/month (instead of ₹9,600)
```

**Trade-off:** Longer polling = slower response. At 15 seconds, farmer taps ON → motor starts in up to 15 seconds instead of 5. Still acceptable for most use cases.

---

## Is Firebase Right for This Project?

| Scale | Verdict | Reason |
|-------|---------|--------|
| 1-400 farmers | ✅ Perfect | Completely free |
| 400-5,000 farmers | ✅ Great | Very low cost (₹2/farmer/month) |
| 5,000-50,000 farmers | ✅ Good | Works fine, optimize polling interval |
| 50,000+ farmers | ⚠️ Evaluate | May want to consider AWS IoT or Google Cloud IoT for better efficiency |

### Why Firebase works well for us:
- **Free tier** covers the entire pilot/POC phase
- **No server to maintain** — Google handles everything
- **Real-time updates** — app shows status instantly
- **Simple REST API** — the STM32 board can talk to it with basic HTTP
- **Scales automatically** — no need to upgrade servers manually
- **Reliable** — Google's infrastructure, 99.95% uptime

---

## Key Terms Explained

| Term | Simple Meaning |
|------|---------------|
| **Download bandwidth** | Like mobile data — how much data is read from the database per month |
| **Storage** | Like hard drive space — how much data is saved in the database |
| **Simultaneous connections** | How many phone apps can be open and listening at the exact same time |
| **Polling** | The board asking "any new command?" every few seconds |
| **REST API** | A way for the board to talk to Firebase using simple web requests (like opening a URL) |
| **Spark plan** | Firebase free plan — no credit card needed |
| **Blaze plan** | Firebase paid plan — pay only for what you use beyond free limits |
| **Real-time listener** | The app stays connected to Firebase and gets instant updates (no polling needed) |

---

## Summary

- **First 400 farmers = FREE** (₹0/month)
- **After that = ₹2 per farmer per month**
- **No server costs, no maintenance, no hardware to buy**
- **Billing resets every month** (like a mobile recharge)
- **Can be optimized further** by increasing polling interval

---

*Document prepared for: Motor Controller Project (motor-controller-be320)*
*Firebase Console: console.firebase.google.com/project/motor-controller-be320*
*Last updated: October 2026*
