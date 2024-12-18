import json
import sys
import matplotlib.pyplot as plt
from reportlab.lib.pagesizes import A4
from reportlab.pdfgen import canvas
import tempfile
import os

def extract_test_setup(json_file_path):
    with open(json_file_path, 'r') as file:
        data = json.load(file)


    # Extract json
    test_results_list = data.get("test_results", [])

    # Create PDF
    pdf_file_path = "testreport.pdf"
    c = canvas.Canvas(pdf_file_path, pagesize=A4)
    width, height = A4

    for index, test_data in enumerate(test_results_list):
        test_title = test_data.get("test_title", "No Title")

        testbed_setup = test_data.get("testbed_setup", {})
        testbed_settings = []
        for key, value in testbed_setup.items():
            testbed_settings.append(f"{key.replace('_', ' ')}: {value}")

        test_setup = test_data.get("test_setup", {})
        test_settings = []
        for key, value in test_setup.items():
            test_settings.append(f"{key.replace('_', ' ')}: {value}")

        test_results = test_data.get("results", {})
        results = []
        for key, value in test_results.items():
            results.append(f"{key.replace('_', ' ')}: {value}")


        # Extract and plot latency histogram
        latency_histogram = test_data.get("latency_histogram", [])
        if latency_histogram:
            latencies = []
            counts = []
            bar_colors = []
            for entry in latency_histogram:
                if entry["latency"] != "lost":
                    latencies.append(str(entry["latency"]))
                    counts.append(entry["count"])
                    bar_colors.append((0.37, 0.52, 0.73)) 
                else:
                    latencies.append("lost")
                    counts.append(entry["count"])
                    bar_colors.append((0.86, 0.36, 0.36)) 

            # Add parameters as text on the graph
            plt.figure(figsize=(10, 6))
            plt.xlabel("Latency (s)")
            plt.ylabel("Count")
            plt.grid(True, linestyle='--', axis='y', zorder=0)
            plt.bar(latencies, counts, label=latencies, color=bar_colors, width=0.9, zorder=3)

            # Save the plot to a temporary file
            with tempfile.NamedTemporaryFile(delete=False, suffix='.png') as tmpfile:
                plt.savefig(tmpfile.name, format='png')
                tmpfile_path = tmpfile.name
            plt.close()


        xPos = 40
        yPos = height - 40

        # Add title to PDF
        c.setFont("Helvetica-Bold", 16)
        c.drawString(xPos, yPos, "Test Report - " + test_title)
        yPos -= 30

        # Add subtitle for Testbed setup
        c.setFont("Helvetica-Bold", 14)
        c.drawString(xPos, yPos, "Testbed Setup")
        yPos -= 20

        text = c.beginText(xPos, yPos)
        text.setFont("Helvetica", 11)
        text.textLines("\n".join(testbed_settings))
        c.drawText(text)
        yPos -= 20 + (len(testbed_settings)*12)

        # Add subtitle for Test setup
        c.setFont("Helvetica-Bold", 14)
        c.drawString(xPos, yPos, "Test Setup")
        yPos -= 20

        text = c.beginText(xPos, yPos)
        text.setFont("Helvetica", 11)
        text.textLines("\n".join(test_settings))
        c.drawText(text)
        yPos -= 20 + (len(test_settings)*12)

        # Add subtitle for Test results
        c.setFont("Helvetica-Bold", 14)
        c.drawString(xPos, yPos, "Test Results")
        yPos -= 20

        text = c.beginText(xPos, yPos)
        text.setFont("Helvetica", 11)
        text.textLines("\n".join(results))
        c.drawText(text)
        yPos -= (len(results)*12)
        yPos -= 20

        if latency_histogram:
            #latency histogram
            c.setFont("Helvetica-Bold", 11)
            c.drawString(xPos, yPos, "Response Time Histogram")
            yPos -= 370

            # Add histogram to PDF at the bottom
            c.drawImage(tmpfile_path, (width-600)/2, yPos, width=600, height=360)

            # Clean up the temporary file
            os.remove(tmpfile_path)
        c.showPage()
    c.save()
    print(f"PDF report saved as {pdf_file_path}")


# Example usage
if len(sys.argv) != 2:
    print("Usage: python testreport.py <json_file_path>")
else:
    json_file_path = sys.argv[1]
    extract_test_setup(json_file_path)