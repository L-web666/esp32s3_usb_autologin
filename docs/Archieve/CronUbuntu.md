
---

# Cron Jobs in Ubuntu

[CronTab Expressions](https://crontab.guru/)

Cron is a time-based job scheduler in Unix-like operating systems, including Ubuntu. It allows you to run commands or scripts at specified times and intervals.

## Scheduling a Cron Job

### 1. Open the Cron Table
To schedule a cron job, you'll need to edit your crontab file (a table of cron jobs) for your user or root.

```bash
crontab -e
```

This will open the crontab file in the default text editor (usually nano). If you want to edit the crontab for a different user, use the following command:

```bash
sudo crontab -e -u username
```

### 2. Add a Cron Job
Each line in the crontab file represents a cron job. The general syntax is:

```
* * * * * /path/to/command
│ │ │ │ │
│ │ │ │ └── Day of the week (0 - 6) (Sunday=0)
│ │ │ │
│ │ │ └──── Month (1 - 12)
│ │ │
│ │ └───── Day of the month (1 - 31)
│ │
│ └────── Hour (0 - 23)
│
└──────── Minute (0 - 59)
```

### 3. Cron Syntax Breakdown
Cron syntax consists of five fields followed by the command to be executed. Here's an explanation of each field:

- **Minute (0 - 59)**: The minute of the hour when the job will run (0-59).
- **Hour (0 - 23)**: The hour of the day when the job will run (0-23). `0` means midnight.
- **Day of the month (1 - 31)**: The day of the month when the job will run (1-31).
- **Month (1 - 12)**: The month when the job will run (1-12).
- **Day of the week (0 - 6)**: The day of the week when the job will run (0-6). `0` or `7` represents Sunday.

### 4. Special Characters

You can use the following special characters to represent certain values:

- **`*` (asterisk)**: Matches any value. For example, `*` in the minute field means every minute.
- **`,` (comma)**: Used to specify multiple values. For example, `1,2,5` in the day-of-week field means Monday, Tuesday, and Friday.
- **`-` (hyphen)**: Specifies a range of values. For example, `1-5` in the day-of-week field means Monday through Friday.
- **`/` (slash)**: Specifies increments. For example, `*/5` in the minute field means "every 5 minutes."

### 5. Examples of Cron Jobs

#### Run a Script Every Day at Midnight
To run a script at midnight every day, add the following cron job:

```
0 0 * * * /path/to/your/script.sh
```

This means:
- **0 minute**
- **0 hour (midnight)**
- **Every day of the month**
- **Every month**
- **Every day of the week**

#### Run a Job Every 10 Minutes
To run a job every 10 minutes, add:

```
*/10 * * * * /path/to/your/script.sh
```

This means:
- **Every 10 minutes**
- **Every hour**
- **Every day of the month**
- **Every month**
- **Every day of the week**

#### Run a Script Every Week on Sunday at 2:30 PM
To run a script every Sunday at 2:30 PM, add:

```
30 14 * * 0 /path/to/your/script.sh
```

This means:
- **30 minutes**
- **14 hours (2:30 PM)**
- **Every day of the month**
- **Every month**
- **Sunday (0)**

#### Run a Job on the 1st Day of Every Month at 3 AM
To run a job on the 1st day of every month at 3 AM, add:

```
0 3 1 * * /path/to/your/script.sh
```

This means:
- **0 minutes**
- **3 hours (3 AM)**
- **1st day of the month**
- **Every month**
- **Every day of the week**

#### Run a Job Every Weekday (Monday to Friday) at 5 PM
To run a job at 5 PM every weekday, add:

```
0 17 * * 1-5 /path/to/your/script.sh
```

This means:
- **0 minutes**
- **17 hours (5 PM)**
- **Every day of the month**
- **Every month**
- **Monday to Friday (1-5)**

### 6. Save and Exit
After adding the cron job, save the file and exit the editor:

- In **nano**, press `CTRL + X`, then `Y` to confirm saving, and `Enter` to exit.

Your cron job is now scheduled!

### 7. List Scheduled Cron Jobs
To list all scheduled cron jobs for your user:

```bash
crontab -l
```

### 8. Remove a Cron Job
To remove a cron job, simply edit the crontab file again (`crontab -e`) and delete the line corresponding to the job you want to remove.

### 9. View Cron Logs
To check the logs of executed cron jobs, you can view the system log:

```bash
grep CRON /var/log/syslog
```

---

## Summary of Common Cron Syntax

| Syntax | Description |
|--------|-------------|
| `*`    | Every value (minute, hour, day, etc.) |
| `*/5`  | Every 5th value (e.g., every 5 minutes) |
| `1-5`  | A range of values (e.g., 1 through 5) |
| `1,3,5`| Multiple specific values (e.g., 1st, 3rd, and 5th) |

---
