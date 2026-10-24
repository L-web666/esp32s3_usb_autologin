
```
* * * * * * * /path/to/command
│ │ │ │ │ │ │
│ │ │ │ │ │ └── Year (1970 - 2099) [optional, defaults to * (every year)]
│ │ │ │ │ │
│ │ │ │ │ └──── Day of the week (0 - 6) (Sunday=0)
│ │ │ │ │
│ │ │ │ └────── Month (1 - 12 or JAN - DEC)
│ │ │ │
│ │ │ └──────── Day of the month (1 - 31)
│ │ │
│ │ └───────── Hour (0 - 23)
│ │
│ └────────── Minute (0 - 59)
│
└─────────── Second (0 - 59) [optional, defaults to 0]
```

---

### **Example Expressions**

#### **Basic Examples**
```
0 * * * * * * /path/to/command
# Every minute starting at the beginning of the minute.
```

```
30 15 * * * * * /path/to/command
# At 15:30 (3:30 PM) every day.
```

```
0 0 12 * * * * /path/to/command
# At 12:00 PM (noon) every day.
```

---

#### **Range and Step Examples**
```
*/15 * * * * * * /path/to/command
# Every 15 seconds.
```

```
0 */10 9-17 * * * * /path/to/command
# Every 10 minutes between 9 AM and 5 PM.
```

```
0 0 12 1-15 1-6 * * /path/to/command
# At noon, on the 1st to the 15th of January to June.
```

---

#### **Year-Specific Examples**
```
0 30 9 1 1 * 2025 /path/to/command
# At 9:30 AM on January 1, 2025.
```

```
0 0 12 * * * 2023-2025 /path/to/command
# At noon every day from 2023 to 2025.
```

---

#### **Complex Patterns**
```
30 15 9 1,15 1 MON 2025 /path/to/command
# At 9:15:30 AM on January 1 and 15, 2025, if it's a Monday.
```

```
0 0 0 1 * * * /path/to/command
# At midnight on the 1st day of every month.
```

---
