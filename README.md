The *Indicators of Anti-Forensics* (IoAF) project is an effort towards
automated anti-forensic trace detection using signature-based methods. 
Each "version" represents the work of different KITRI Best of the Best
groups to advance the idea.

The main IoAF program uses parsing modules to extract file meta-data
and Registry key information from a system under investigation. 
Pre-defined signatures are stored in a SQLite database that is queried 
for each extracted object.

Signatures are created by using either real-time or snapshot based 
analysis on a similar system. Objects that are consistently updated 
by the action of interest are extracted, and further tested (e.g. 
how the object is updated). If the object is found to consistently 
correspond to the action of interest - and only the action of interest 
- it is included as a trace in the signature.

The purpose of the project so far is not to automatically reconstruct 
activities, but to quickly detect the presence of anti-forensic traces 
to let investigators know whether they should pay more interest to this 
device over others (digital forensic triage).

## General Schedule
IoAFv1 - Initial proof of concept. Used C# with a number of external programs 
such as The Sleuth Kit (http://www.sleuthkit.org/) to parse various file 
systems and compare traces with basic signatures of known anti-forensic programs.

IoAFv2 - Version two worked towards improving the speed of the tool for practical 
use by investigators in the field. A code rewrite was done in C++. Parsing 'modules' 
were introduced, of which MFT and Registry parsing was completed. The reporting interface 
was also improved, written in HTML/JS.


## Contributors
IoAFv1: Kyoung Jea Park, Jung-Min Park, Eunjin Kim, Chang Geun Cheon

IoAFv2: Moon Seong Kim, JaeYoung Choi, Sang Seob Lee, Eunjin Kim

## Related Publications
* James, J. I., Gladyshev, P., & Zhu, Y. (2011). Signature Based Detection of User 
Events for Post-Mortem Forensic Analysis. Digital Forensics and Cyber Crime, 53, 
96-109. doi:10.1007/978-3-642-19513-6_8
* James, J.I., P. Gladyshev (2014) "Automated Inference of Past Action Instances 
in Digital Investigations." International Journal of Information Security. Springer. 
10.1007/s10207-014-0249-6.

## License
Please fork this project. If you could like to contribute more directly, please contact us.
This project is released under a GNU General Public License v3 (https://www.gnu.org/licenses/gpl-3.0.html)

