#!/usr/bin/env python3
"""
Quick Logbook Entry Tool for Desk Buddy
Appends a timestamped log entry directly into Logbook/PROJECT_LOGBOOK.md
"""

import os
import sys
from datetime import datetime

LOGBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "PROJECT_LOGBOOK.md")

def create_entry_interactive():
    print("=" * 60)
    print("🤖 Desk Buddy Project - New Logbook Entry")
    print("=" * 60)
    
    now = datetime.now()
    timestamp_str = now.strftime("%Y-%m-%d | %I:%M %p")
    print(f"Timestamp: {timestamp_str}\n")
    
    title = input("Entry Title / Milestone (e.g., Tested I2C OLED screen): ").strip()
    if not title:
        title = "Progress Update"
        
    objective = input("Objective: ").strip() or "General development and testing"
    actions = input("Actions Taken: ").strip() or "Work on hardware and sketch"
    observations = input("Observations / Results: ").strip() or "Components functioning as expected"
    issues = input("Issues / Blockers (Press Enter if none): ").strip() or "None"
    next_steps = input("Next Steps: ").strip() or "Continue hardware assembly and feature additions"
    logged_by = input("Logged By (Name/s): ").strip() or "Desk Buddy Team"
    
    entry_text = f"""
### [{timestamp_str}] - {title}
- **Objective:** {objective}
- **Actions Taken:** {actions}
- **Observations / Results:** {observations}
- **Issues / Blockers Encountered:** {issues}
- **Next Steps:** {next_steps}
- **Logged By:** {logged_by}

---
"""
    
    with open(LOGBOOK_PATH, "a", encoding="utf-8") as f:
        f.write(entry_text)
        
    print("\n✅ Successfully appended new entry to PROJECT_LOGBOOK.md!")
    print(f"Path: {LOGBOOK_PATH}")

if __name__ == "__main__":
    create_entry_interactive()
