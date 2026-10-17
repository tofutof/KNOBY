#!/usr/bin/env python3
"""
=====================================================================
 DESK BUDDY - CASE STUDY REPORT & PDF PROMPTER
=====================================================================
 Generates a polished, professional Case Study PDF following the
 exact required format:

   TYPE MEMBERS NAME
   1. updated image of circuit design (WOKWI/Tinkercad simulation if any/drawing/diagram)
   2. picture/s of the finished project
   3. list of materials/components/wiring config
   4. estimated cost of the project components used
   5. full working code
   6. short video test (can be in link)
   7. short description of the project functionality.
=====================================================================
"""

import os
import sys
import json
import base64
import subprocess
import html
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent
WORKSPACE_DIR = BASE_DIR.parent
DATA_FILE = BASE_DIR / "case_study_data.json"
HTML_OUTPUT = BASE_DIR / "Final_Case_Study.html"
PDF_OUTPUT = BASE_DIR / "Final_Case_Study.pdf"
DEFAULT_INO_FILE = WORKSPACE_DIR / "Project" / "DeskBuddy" / "DeskBuddy.ino"

CHROME_PATHS = [
    "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
    "/Applications/Chromium.app/Contents/MacOS/Chromium",
    "/Applications/Brave Browser.app/Contents/MacOS/Brave Browser",
    "/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge"
]

def find_chrome():
    for p in CHROME_PATHS:
        if os.path.exists(p):
            return p
    # Fallback to PATH lookup
    for cmd in ["google-chrome", "chromium", "chromium-browser"]:
        res = subprocess.run(["which", cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if res.returncode == 0 and res.stdout.strip():
            return res.stdout.strip()
    return None

def image_to_base64_data_uri(image_path_str):
    if not image_path_str:
        return ""
    # Check absolute or relative to BASE_DIR or WORKSPACE_DIR
    p = Path(image_path_str)
    if not p.is_absolute():
        if (BASE_DIR / p).exists():
            p = BASE_DIR / p
        elif (WORKSPACE_DIR / p).exists():
            p = WORKSPACE_DIR / p

    if p.exists() and p.is_file():
        suffix = p.suffix.lower()
        mime_types = {
            ".png": "image/png",
            ".jpg": "image/jpeg",
            ".jpeg": "image/jpeg",
            ".webp": "image/webp",
            ".svg": "image/svg+xml",
            ".gif": "image/gif"
        }
        mime = mime_types.get(suffix, "image/png")
        try:
            with open(p, "rb") as img_f:
                encoded = base64.b64encode(img_f.read()).decode("utf-8")
                return f"data:{mime};base64,{encoded}"
        except Exception:
            return ""
    return ""

def load_data():
    if DATA_FILE.exists():
        try:
            with open(DATA_FILE, "r", encoding="utf-8") as f:
                return json.load(f)
        except Exception as e:
            print(f"⚠️ Warning loading {DATA_FILE}: {e}")
    return {}

def save_data(data):
    with open(DATA_FILE, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)

def interactive_prompt(existing_data):
    print("\n" + "=" * 65)
    print("📋 CASE STUDY DATA PROMPTER - DESK BUDDY")
    print("=" * 65)
    print("Kindly provide the following data of your final case study:\n")

    data = dict(existing_data) if existing_data else {}

    # Title
    current_title = data.get("project_title", "Desk Buddy: Interactive Desktop Companion")
    inp = input(f"Project Title [{current_title}]: ").strip()
    data["project_title"] = inp if inp else current_title

    # Members
    print("\n👥 TYPE MEMBERS NAME:")
    default_members = ", ".join(data.get("members", ["Member 1", "Member 2"]))
    inp = input(f"Enter member names (separated by commas) [{default_members}]: ").strip()
    if inp:
        data["members"] = [m.strip() for m in inp.split(",") if m.strip()]
    elif "members" not in data or not data["members"]:
        data["members"] = ["Desk Buddy Team"]

    # 1. Circuit Design Image & Simulation Link
    print("\n[1] Updated image of circuit design (WOKWI/Tinkercad simulation if any/drawing/diagram):")
    sec1 = data.get("section_1_circuit_design", {})
    curr_sim = sec1.get("simulation_link", "")
    inp_sim = input(f"Simulation link (WOKWI / Tinkercad link) [{curr_sim}]: ").strip()
    sec1["simulation_link"] = inp_sim if inp_sim else curr_sim

    curr_img = sec1.get("image_path", "assets/circuit_diagram.png")
    inp_img = input(f"Circuit design image path [{curr_img}]: ").strip()
    sec1["image_path"] = inp_img if inp_img else curr_img
    
    curr_desc = sec1.get("description", "Circuit schematic showing Arduino UNO, 0.96\" I2C OLED display, and 9V battery connection.")
    inp_desc = input(f"Circuit description [{curr_desc}]: ").strip()
    sec1["description"] = inp_desc if inp_desc else curr_desc
    data["section_1_circuit_design"] = sec1

    # 2. Picture/s of the Finished Project
    print("\n[2] Picture/s of the finished project:")
    sec2 = data.get("section_2_finished_project_photos", {})
    curr_photos = ", ".join(sec2.get("image_paths", ["assets/finished_project_1.jpg"]))
    inp_photos = input(f"Finished project image paths (comma-separated) [{curr_photos}]: ").strip()
    if inp_photos:
        sec2["image_paths"] = [p.strip() for p in inp_photos.split(",") if p.strip()]
    else:
        sec2["image_paths"] = sec2.get("image_paths", ["assets/finished_project_1.jpg"])
    data["section_2_finished_project_photos"] = sec2

    # 3. Materials & Components / Wiring Config
    print("\n[3] List of materials/components/wiring config:")
    print("   (Using structured component list & wiring table. Press enter to keep defaults or edit case_study_data.json)")
    if "section_3_materials_and_wiring" not in data:
        data["section_3_materials_and_wiring"] = {
            "components": [
                {"item": "Arduino UNO R3", "quantity": "1 pc", "description": "Main microcontroller board"},
                {"item": "0.96\" I2C OLED Display (SSD1306)", "quantity": "1 pc", "description": "128x64 display"},
                {"item": "9V Battery", "quantity": "1 pc", "description": "Power source"},
                {"item": "9V Battery Clip with DC Jack", "quantity": "1 pc", "description": "Power connection to UNO"},
                {"item": "Breadboard & Jumper Wires", "quantity": "1 set", "description": "I2C and power connections"}
            ],
            "wiring_config": [
                {"oled_pin": "GND", "arduino_pin": "GND", "details": "Ground"},
                {"oled_pin": "VCC", "arduino_pin": "5V", "details": "Power (5V)"},
                {"oled_pin": "SCL", "arduino_pin": "A5", "details": "I2C Clock line"},
                {"oled_pin": "SDA", "arduino_pin": "A4", "details": "I2C Data line"},
                {"oled_pin": "9V Battery (+)", "arduino_pin": "VIN / DC Jack", "details": "Power Supply"},
                {"oled_pin": "9V Battery (-)", "arduino_pin": "GND", "details": "Power Ground"}
            ]
        }

    # 4. Estimated Cost of Components
    print("\n[4] Estimated cost of the project components used:")
    sec4 = data.get("section_4_estimated_cost", {})
    curr_cost = sec4.get("total_estimated_cost", "$14.50 (Approx. ₱800 - ₱900)")
    inp_cost = input(f"Total estimated cost summary [{curr_cost}]: ").strip()
    sec4["total_estimated_cost"] = inp_cost if inp_cost else curr_cost
    data["section_4_estimated_cost"] = sec4

    # 5. Full Working Code
    print("\n[5] Full working code:")
    sec5 = data.get("section_5_working_code", {})
    curr_src = sec5.get("source_file", "../Project/DeskBuddy/DeskBuddy.ino")
    inp_src = input(f"Code source file path [{curr_src}]: ").strip()
    sec5["source_file"] = inp_src if inp_src else curr_src
    data["section_5_working_code"] = sec5

    # 6. Short Video Test
    print("\n[6] Short video test (can be in link):")
    sec6 = data.get("section_6_video_test", {})
    curr_vid = sec6.get("video_link", "https://youtu.be/your_video_id_here")
    inp_vid = input(f"Video test link (YouTube/Drive/Loom) [{curr_vid}]: ").strip()
    sec6["video_link"] = inp_vid if inp_vid else curr_vid
    data["section_6_video_test"] = sec6

    # 7. Short Description of the Project Functionality
    print("\n[7] Short description of the project functionality:")
    curr_desc7 = data.get("section_7_project_description", "")
    if not curr_desc7:
        curr_desc7 = "The Desk Buddy is an interactive smart desk companion built using an Arduino UNO and a 0.96-inch monochrome I2C OLED display (SSD1306). Powered portably by a 9V battery, it renders expressive animated facial states including happy, blink, look left/right, wink, and sleepy modes."
    print(f"Current description:\n{curr_desc7}")
    inp_desc7 = input("Press ENTER to keep or type new description: ").strip()
    data["section_7_project_description"] = inp_desc7 if inp_desc7 else curr_desc7

    save_data(data)
    print("\n✅ Case study data saved to case_study_data.json")
    return data

def resolve_code_content(sec5):
    source_file = sec5.get("source_file", "")
    code_text = sec5.get("code_snippet", "")
    if source_file:
        p = Path(source_file)
        if not p.is_absolute():
            if (BASE_DIR / p).exists():
                p = BASE_DIR / p
            elif (WORKSPACE_DIR / p).exists():
                p = WORKSPACE_DIR / p
        if p.exists() and p.is_file():
            try:
                with open(p, "r", encoding="utf-8") as f:
                    return f.read()
            except Exception:
                pass
    if code_text:
        return code_text
    if DEFAULT_INO_FILE.exists():
        try:
            with open(DEFAULT_INO_FILE, "r", encoding="utf-8") as f:
                return f.read()
        except Exception:
            pass
    return "// Arduino code not found. Please verify file path in case_study_data.json."

def build_html_report(data):
    title = html.escape(data.get("project_title", "Desk Buddy Project Case Study"))
    members = data.get("members", ["Team Members"])
    members_html = ", ".join([f"<strong>{html.escape(m)}</strong>" for m in members])

    # Section 1
    sec1 = data.get("section_1_circuit_design", {})
    sec1_desc = html.escape(sec1.get("description", ""))
    sec1_sim = sec1.get("simulation_link", "")
    sec1_img_path = sec1.get("image_path", "")
    sec1_img_b64 = image_to_base64_data_uri(sec1_img_path)

    # Section 2
    sec2 = data.get("section_2_finished_project_photos", {})
    sec2_desc = html.escape(sec2.get("description", ""))
    sec2_imgs = sec2.get("image_paths", [])

    # Section 3
    sec3 = data.get("section_3_materials_and_wiring", {})
    components = sec3.get("components", [])
    wiring = sec3.get("wiring_config", [])

    # Section 4
    sec4 = data.get("section_4_estimated_cost", {})
    cost_items = sec4.get("items", [])
    total_cost = html.escape(sec4.get("total_estimated_cost", "N/A"))

    # Section 5
    sec5 = data.get("section_5_working_code", {})
    code_raw = resolve_code_content(sec5)
    code_escaped = html.escape(code_raw)

    # Section 6
    sec6 = data.get("section_6_video_test", {})
    video_link = sec6.get("video_link", "")
    video_notes = html.escape(sec6.get("notes", ""))

    # Section 7
    sec7_desc = html.escape(data.get("section_7_project_description", ""))

    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>{title} - Final Case Study</title>
  <style>
    @page {{
      size: A4 portrait;
      margin: 15mm 15mm 15mm 15mm;
    }}
    * {{
      box-sizing: border-box;
      -webkit-print-color-adjust: exact !important;
      print-color-adjust: exact !important;
    }}
    body {{
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      line-height: 1.5;
      color: #1a202c;
      background-color: #ffffff;
      margin: 0;
      padding: 0;
      font-size: 13.5px;
    }}
    .container {{
      width: 100%;
      max-width: 820px;
      margin: 0 auto;
    }}
    .header {{
      border-bottom: 2.5px solid #2b6cb0;
      padding-bottom: 14px;
      margin-bottom: 22px;
    }}
    .header h1 {{
      margin: 0 0 6px 0;
      font-size: 24px;
      color: #1a365d;
      letter-spacing: -0.5px;
      text-transform: uppercase;
    }}
    .subtitle {{
      color: #4a5568;
      font-size: 14px;
      margin-bottom: 8px;
    }}
    .members-badge {{
      display: inline-block;
      background-color: #ebf8ff;
      border: 1px solid #bee3f8;
      color: #2b6cb0;
      padding: 6px 12px;
      border-radius: 6px;
      font-size: 13px;
      margin-top: 4px;
    }}
    .section-card {{
      background: #ffffff;
      border: 1px solid #e2e8f0;
      border-radius: 8px;
      padding: 16px 18px;
      margin-bottom: 20px;
      page-break-inside: avoid;
    }}
    .section-title {{
      margin-top: 0;
      margin-bottom: 12px;
      font-size: 16px;
      color: #2c5282;
      border-bottom: 1px solid #edf2f7;
      padding-bottom: 6px;
      display: flex;
      align-items: center;
      gap: 8px;
    }}
    .section-num {{
      background: #2b6cb0;
      color: white;
      font-size: 12px;
      padding: 2px 7px;
      border-radius: 12px;
      font-weight: bold;
    }}
    table {{
      width: 100%;
      border-collapse: collapse;
      margin: 10px 0;
      font-size: 12.5px;
    }}
    th, td {{
      padding: 7px 10px;
      text-align: left;
      border: 1px solid #e2e8f0;
    }}
    th {{
      background-color: #f7fafc;
      color: #2d3748;
      font-weight: 600;
    }}
    tr:nth-child(even) {{
      background-color: #f8fafc;
    }}
    .img-box {{
      text-align: center;
      margin: 12px 0;
      background: #f7fafc;
      border: 1px dashed #cbd5e0;
      border-radius: 6px;
      padding: 12px;
    }}
    .img-box img {{
      max-width: 100%;
      max-height: 280px;
      object-fit: contain;
      border-radius: 4px;
      border: 1px solid #e2e8f0;
    }}
    .img-placeholder {{
      color: #718096;
      font-style: italic;
      padding: 25px 15px;
      font-size: 12.5px;
    }}
    .link-button {{
      display: inline-block;
      background: #3182ce;
      color: #ffffff !important;
      text-decoration: none;
      padding: 6px 12px;
      border-radius: 4px;
      font-size: 12.5px;
      font-weight: 500;
      margin-top: 6px;
    }}
    .code-container {{
      background-color: #1a202c;
      color: #e2e8f0;
      padding: 12px 14px;
      border-radius: 6px;
      font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, Courier, monospace;
      font-size: 11px;
      line-height: 1.42;
      overflow-x: auto;
      white-space: pre;
      max-height: 480px;
      page-break-inside: auto;
    }}
    .total-cost-banner {{
      margin-top: 10px;
      background: #feebc8;
      border: 1px solid #fbd38d;
      color: #744210;
      padding: 8px 12px;
      border-radius: 6px;
      font-weight: 600;
      text-align: right;
    }}
    .footer-note {{
      font-size: 11px;
      color: #a0aec0;
      text-align: center;
      margin-top: 25px;
      border-top: 1px solid #e2e8f0;
      padding-top: 8px;
    }}
  </style>
</head>
<body>
  <div class="container">
    
    <!-- HEADER & MEMBERS -->
    <div class="header">
      <h1>{title}</h1>
      <div class="subtitle">Final Case Study & Engineering Report</div>
      <div class="members-badge">
        👥 <strong>MEMBERS:</strong> {members_html}
      </div>
    </div>

    <!-- 1. UPDATED IMAGE OF CIRCUIT DESIGN -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">1</span>
        Updated Image of Circuit Design (WOKWI / Tinkercad / Diagram)
      </h2>
      <p>{sec1_desc}</p>
      {f'<p><strong>Simulation Link:</strong> <a href="{sec1_sim}" target="_blank">{sec1_sim}</a></p>' if sec1_sim else ''}
      <div class="img-box">
        {f'<img src="{sec1_img_b64}" alt="Circuit Design Diagram">' if sec1_img_b64 else f'<div class="img-placeholder">📷 [Circuit Diagram Image Placeholder]<br>Place image at <code>{html.escape(sec1_img_path or "assets/circuit_diagram.png")}</code> to embed automatically.</div>'}
      </div>
    </div>

    <!-- 2. PICTURE/S OF FINISHED PROJECT -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">2</span>
        Picture/s of the Finished Project
      </h2>
      <p>{sec2_desc}</p>
      <div class="img-box">
"""
    # Section 2 images
    sec2_has_img = False
    for img_path in sec2_imgs:
        b64 = image_to_base64_data_uri(img_path)
        if b64:
            sec2_has_img = True
            html_content += f'<div style="margin-bottom:10px;"><img src="{b64}" alt="Finished Project Photo"></div>\n'
    if not sec2_has_img:
        placeholder_paths = ", ".join([html.escape(p) for p in sec2_imgs]) or "assets/finished_project_1.jpg"
        html_content += f'<div class="img-placeholder">📸 [Finished Project Photo Placeholder]<br>Place photo at <code>{placeholder_paths}</code> to embed automatically.</div>\n'
    
    html_content += """
      </div>
    </div>

    <!-- 3. LIST OF MATERIALS / COMPONENTS / WIRING CONFIG -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">3</span>
        List of Materials / Components / Wiring Config
      </h2>
      
      <h3 style="font-size:13.5px; color:#2d3748; margin: 12px 0 6px 0;">A. Bill of Materials</h3>
      <table>
        <thead>
          <tr>
            <th style="width: 35%;">Component / Item</th>
            <th style="width: 15%;">Quantity</th>
            <th style="width: 50%;">Specification / Role</th>
          </tr>
        </thead>
        <tbody>
"""
    for comp in components:
        c_item = html.escape(comp.get("item", ""))
        c_qty = html.escape(comp.get("quantity", ""))
        c_desc = html.escape(comp.get("description", ""))
        html_content += f"""          <tr>
            <td><strong>{c_item}</strong></td>
            <td>{c_qty}</td>
            <td>{c_desc}</td>
          </tr>\n"""

    html_content += """        </tbody>
      </table>

      <h3 style="font-size:13.5px; color:#2d3748; margin: 14px 0 6px 0;">B. Wiring Configuration (I2C & Power)</h3>
      <table>
        <thead>
          <tr>
            <th style="width: 30%;">Module / Source Terminal</th>
            <th style="width: 30%;">Arduino UNO Pin</th>
            <th style="width: 40%;">Connection Details</th>
          </tr>
        </thead>
        <tbody>
"""
    for w in wiring:
        w_oled = html.escape(w.get("oled_pin", ""))
        w_ard = html.escape(w.get("arduino_pin", ""))
        w_det = html.escape(w.get("details", ""))
        html_content += f"""          <tr>
            <td><strong>{w_oled}</strong></td>
            <td><code style="background:#edf2f7; padding:2px 5px; border-radius:3px;">{w_ard}</code></td>
            <td>{w_det}</td>
          </tr>\n"""

    html_content += f"""        </tbody>
      </table>
    </div>

    <!-- 4. ESTIMATED COST OF COMPONENTS -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">4</span>
        Estimated Cost of the Project Components Used
      </h2>
      <table>
        <thead>
          <tr>
            <th style="width: 55%;">Component Item</th>
            <th style="width: 25%;">Unit Cost</th>
            <th style="width: 20%;">Total Cost</th>
          </tr>
        </thead>
        <tbody>
"""
    for cost in cost_items:
        ci_item = html.escape(cost.get("item", ""))
        ci_unit = html.escape(cost.get("unit_cost", ""))
        ci_tot = html.escape(cost.get("total_cost", ""))
        html_content += f"""          <tr>
            <td>{ci_item}</td>
            <td>{ci_unit}</td>
            <td><strong>{ci_tot}</strong></td>
          </tr>\n"""

    html_content += f"""        </tbody>
      </table>
      <div class="total-cost-banner">
        💰 Overall Estimated Project Cost: {total_cost}
      </div>
    </div>

    <!-- 5. FULL WORKING CODE -->
    <div class="section-card" style="page-break-inside: auto;">
      <h2 class="section-title">
        <span class="section-num">5</span>
        Full Working Code
      </h2>
      <p style="font-size:12px; color:#4a5568; margin-bottom:8px;">
        Firmware sketch targeting ATmega328P with SSD1306 OLED driver:
      </p>
      <pre class="code-container"><code>{code_escaped}</code></pre>
    </div>

    <!-- 6. SHORT VIDEO TEST -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">6</span>
        Short Video Test
      </h2>
      <p>Demonstration video showcasing the Desk Buddy hardware startup, OLED facial expressions, and battery-powered portable operation.</p>
      {f'<p><strong>Video Link:</strong> <a class="link-button" href="{video_link}" target="_blank">▶ Watch Demonstration Video ({video_link})</a></p>' if video_link else '<p><em>No video link provided yet.</em></p>'}
      {f'<p style="font-size:12.5px; color:#4a5568; margin-top:6px;"><strong>Notes:</strong> {video_notes}</p>' if video_notes else ''}
    </div>

    <!-- 7. SHORT DESCRIPTION OF THE PROJECT FUNCTIONALITY -->
    <div class="section-card">
      <h2 class="section-title">
        <span class="section-num">7</span>
        Short Description of the Project Functionality
      </h2>
      <div style="font-size:13.5px; line-height:1.65; color:#2d3748; background:#f7fafc; border-left:4px solid #2b6cb0; padding:12px 16px; border-radius:0 6px 6px 0;">
        {sec7_desc}
      </div>
    </div>

    <div class="footer-note">
      Generated automatically by Desk Buddy Case Study Prompter | Arduino UNO &bull; 0.96" OLED &bull; 9V Battery
    </div>

  </div>
</body>
</html>
"""
    return html_content

def generate_pdf_from_html(html_file, pdf_file):
    chrome_bin = find_chrome()
    if not chrome_bin:
        print("⚠️ Chrome/Chromium executable not found. Cannot automatically convert HTML to PDF via headless browser.")
        print(f"👉 You can open {html_file} in your browser and choose 'File -> Print -> Save as PDF'.")
        return False

    cmd = [
        chrome_bin,
        "--headless",
        "--disable-gpu",
        f"--print-to-pdf={pdf_file}",
        "--no-pdf-header-footer",
        str(html_file.resolve().as_uri())
    ]
    try:
        res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if pdf_file.exists() and pdf_file.stat().st_size > 0:
            return True
        else:
            print(f"⚠️ Chrome exited with error: {res.stderr}")
            return False
    except Exception as e:
        print(f"⚠️ Error running Chrome headless: {e}")
        return False

def build_report():
    data = load_data()
    if not data:
        print("No data found. Running interactive prompter...")
        data = interactive_prompt({})

    html_content = build_html_report(data)
    with open(HTML_OUTPUT, "w", encoding="utf-8") as f:
        f.write(html_content)
    print(f"✅ HTML report generated: {HTML_OUTPUT}")

    pdf_success = generate_pdf_from_html(HTML_OUTPUT, PDF_OUTPUT)
    if pdf_success:
        print(f"🎉 PDF report successfully created: {PDF_OUTPUT}")
        print(f"   Size: {PDF_OUTPUT.stat().st_size:,} bytes")
    else:
        print(f"ℹ️ Open HTML to print: open {HTML_OUTPUT}")

def main():
    if "--interactive" in sys.argv:
        existing = load_data()
        interactive_prompt(existing)
        build_report()
    elif "--build" in sys.argv or "--file" in sys.argv:
        build_report()
    elif "--open" in sys.argv:
        if PDF_OUTPUT.exists():
            subprocess.run(["open", str(PDF_OUTPUT)])
        elif HTML_OUTPUT.exists():
            subprocess.run(["open", str(HTML_OUTPUT)])
    else:
        # Default menu
        print("=" * 60)
        print("🤖 Desk Buddy - Case Study PDF Generator & Prompter")
        print("=" * 60)
        print("Choose an option:")
        print("  [1] Interactive Prompter (Input / Review all 7 items)")
        print("  [2] Build PDF from case_study_data.json immediately")
        print("  [3] Open existing generated PDF")
        choice = input("Enter choice (1, 2, or 3) [default 2]: ").strip()
        
        if choice == "1":
            existing = load_data()
            interactive_prompt(existing)
            build_report()
        elif choice == "3":
            if PDF_OUTPUT.exists():
                subprocess.run(["open", str(PDF_OUTPUT)])
            else:
                print("PDF not generated yet. Building now...")
                build_report()
        else:
            build_report()

if __name__ == "__main__":
    main()
