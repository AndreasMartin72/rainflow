# Rainflow Counting Algorithm (4-point-method), C99 compliant
  
"Rainflow Counting" consists of four main steps:
  1. Hysteresis Filtering
  2. Peak-Valley Filtering
  3. Discretization
  4. Four Point Counting Method:
```
                     * D
                    / \       Closed, if min(B,C) >= min(A,D) && max(B,C) <= max(A,D)
             B *<--/          Slope B-C is counted and removed from residue
              / \ /
             /   * C
          \ /
           * A
```
These steps are fully documented in standards such as 
ASTM E1049 "Standard Practices for Cycle Counting in Fatigue Analysis" [1].
This implementation uses the 4-point algorithm mentioned in [3,4] and the 3-point HCM method proposed in [2].
To take the residue into account, you may implement a custom method or use some
predefined functions.
 
---
### References:
[1] "Standard Practices for Cycle Counting in Fatigue Analysis."
    ASTM Standard E 1049, 1985 (2011). 
    West Conshohocken, PA: ASTM International, 2011.
[2] "Rainflow - HCM / Ein Hysteresisschleifen-Zaehlalgorithmus auf werkstoffmechanischer Grundlage"
    U.H. Clormann, T. Seeger
    1985 TU Darmstadt, Fachgebiet Werkstoffmechanik
[3] "Zaehlverfahren zur Bildung von Kollektiven und Matrizen aus Zeitfunktionen"
    FVA-Richtlinie, 2010.
    [https://fva-net.de/fileadmin/content/Richtlinien/FVA-Richtlinie_Zaehlverfahren_2010.pdf]
[4] Siemens Product Lifecycle Management Software Inc., 2018. 
    [https://community.plm.automation.siemens.com/t5/Testing-Knowledge-Base/Rainflow-Counting/ta-p/383093]
[5] "Review and application of Rainflow residue processing techniques for accurate fatigue damage estimation"
    G.Marsh;
    International Journal of Fatigue 82 (2016) 757-765,
    [https://doi.org/10.1016/j.ijfatigue.2015.10.007]
[6] "Betriebsfestigkeit - Verfahren und Daten zur Bauteilberechnung"
    Haibach, Erwin; Springer Verlag
[]  "Schaedigungsbasierte Hysteresefilter"; Hack, M, D386 (Diss Univ. Kaiserslautern), Shaker Verlag Aachen, 1998, ISBN 3-8265-3936-2
[]  "Hysteresis and Phase Transition"
    Brokate, M.; Sprekels, J.; Applied Mathematical Sciences 121; Springer, New York, 1996
[]  "Rainflow counting and energy dissipation in elastoplasticity"; Eur. J. Mech. A/Solids 15, pp. 705-737, 1996
    Brokate, M.; Dressler, K.; Krejci, P.
[]  "Multivariate Density Estimation: Theory, Practice and Visualization". New York, Chichester, Wiley & Sons, 1992
    Scott, D.
[]  "Werkstoffmechanik - Bauteile sicher beurteilen undWerkstoffe richtig einsetzen"; 
     Ralf Buergel, Hans Albert Richard, Andre Riemer; Springer FachmedienWiesbaden 2005, 2014
[]  "Zaelverfahren und Lastannahme in der Betriebsfestigkeit";
    Michael Koehler, Sven Jenne / Kurt Poetter, Harald Zenner; Springer-Verlag Berlin Heidelberg 2012
