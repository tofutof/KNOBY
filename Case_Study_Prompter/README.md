# 🖨️ Desk Buddy - Case Study PDF Prompter

This folder contains the automated report generator and terminal prompter to produce the **Final Case Study PDF** adhering to all required case study specifications.

## 📋 Required Case Study Items Handled
- **TYPE MEMBERS NAME**
- **1.** Updated image of circuit design (WOKWI/Tinkercad simulation if any/drawing/diagram)
- **2.** Picture/s of the finished project
- **3.** List of materials/components/wiring config
- **4.** Estimated cost of the project components used
- **5.** Full working code (auto-imported from `Project/DeskBuddy/DeskBuddy.ino`)
- **6.** Short video test (can be in link)
- **7.** Short description of the project functionality

---

## 🚀 How to Run the Prompter

### Option A: Interactive Wizard (Terminal Prompt)
Run the script interactively. It will prompt you step-by-step for member names, image paths, video link, etc.:
```bash
python3 Case_Study_Prompter/prompter.py
```
Or directly with the flag:
```bash
python3 Case_Study_Prompter/prompter.py --interactive
```

### Option B: Quick Build from File
You can simply edit [`case_study_data.json`](./case_study_data.json) directly in your editor and run:
```bash
python3 Case_Study_Prompter/prompter.py --build
```

### Option C: View the Generated PDF
To open the compiled PDF directly in macOS Preview:
```bash
python3 Case_Study_Prompter/prompter.py --open
# or
open Case_Study_Prompter/Final_Case_Study.pdf
```

---

## 🖼️ Adding Pictures & Diagrams
Drop your image files into the [`assets/`](./assets/) folder:
- Circuit diagram: `Case_Study_Prompter/assets/circuit_diagram.png`
- Finished prototype photo: `Case_Study_Prompter/assets/finished_project_1.jpg`

The script automatically encodes local images to high-resolution base64 data URIs so that they render crisp and inline in the final vector PDF!
