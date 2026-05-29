const PFM_SCHEMA = {
    "VoronoiTSPPFM": {
        "name": "Voronoi T S P",
        "params": [
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "mergeTSPPaths",
                "label": "Mergetsppaths",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "VoronoiTreePFM": {
        "name": "Voronoi Tree",
        "params": [
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "createCurves",
                "label": "Createcurves",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "VoronoiDashesPFM": {
        "name": "Voronoi Dashes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "SketchLinesPFM": {
        "name": "Sketch Lines",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            }
        ]
    },
    "SketchCurvesPFM": {
        "name": "Sketch Curves",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "curveTension",
                "label": "Curvetension",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "SketchSuperformulaPFM": {
        "name": "Sketch Superformula",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "centreX",
                "label": "Centrex",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "centreY",
                "label": "Centrey",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "xScale",
                "label": "Xscale",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "yScale",
                "label": "Yscale",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "frequency",
                "label": "Frequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "sineFactor",
                "label": "Sinefactor",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "cosFactor",
                "label": "Cosfactor",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "SketchQuadBeziersPFM": {
        "name": "Sketch Quad Beziers",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "curveTests",
                "label": "Curvetests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 15
            },
            {
                "id": "curveVariation",
                "label": "Curvevariation",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "curveOffset",
                "label": "Curveoffset",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            }
        ]
    },
    "SketchShapesPFM": {
        "name": "Sketch Shapes",
        "params": []
    },
    "SketchFlowFieldPFM": {
        "name": "Sketch Flow Field",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "xFrequency",
                "label": "Xfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "yFrequency",
                "label": "Yfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "scaleFrequency",
                "label": "Scalefrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "amplitude",
                "label": "Amplitude",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "AdaptiveShapesPFM": {
        "name": "Adaptive Shapes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "AdaptiveDiagramPFM": {
        "name": "Adaptive Diagram",
        "params": []
    },
    "AdaptiveTriangulationPFM": {
        "name": "Adaptive Triangulation",
        "params": [
            {
                "id": "minSampleRadius",
                "label": "Minsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "maxSampleRadius",
                "label": "Maxsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "brightness",
                "label": "Brightness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "triangulateCorners",
                "label": "Triangulatecorners",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "AdaptiveDashesPFM": {
        "name": "Adaptive Dashes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "AdaptiveTSPPFM": {
        "name": "Adaptive T S P",
        "params": [
            {
                "id": "minSampleRadius",
                "label": "Minsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "maxSampleRadius",
                "label": "Maxsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "brightness",
                "label": "Brightness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "mergeTSPPaths",
                "label": "Mergetsppaths",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "LBGStipplingPFM": {
        "name": "L B G Stippling",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "stippleSize",
                "label": "Stipplesize",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            }
        ]
    },
    "LBGShapesPFM": {
        "name": "L B G Shapes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "LBGTreePFM": {
        "name": "L B G Tree",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "createCurves",
                "label": "Createcurves",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "LBGLettersPFM": {
        "name": "L B G Letters",
        "params": [
            {
                "id": "word",
                "label": "Word",
                "type": "select",
                "options": [
                    "dbv3"
                ],
                "val": "dbv3"
            },
            {
                "id": "fontSize",
                "label": "Font Size",
                "type": "range",
                "min": 1,
                "max": 100,
                "step": 1,
                "val": 10
            },
            {
                "id": "letterSpacing",
                "label": "Letter Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "wordSpacing",
                "label": "Word Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "lineHeight",
                "label": "Line Height",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "fontName",
                "label": "Font Name",
                "type": "select",
                "options": [
                    "hershey_sans"
                ],
                "val": "hershey_sans"
            }
        ]
    },
    "LBGQuadTilesPFM": {
        "name": "L B G Quad Tiles",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            }
        ]
    },
    "GridShapesPFM": {
        "name": "Grid Shapes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "GridLettersPFM": {
        "name": "Grid Letters",
        "params": [
            {
                "id": "word",
                "label": "Word",
                "type": "select",
                "options": [
                    "dbv3"
                ],
                "val": "dbv3"
            },
            {
                "id": "fontSize",
                "label": "Font Size",
                "type": "range",
                "min": 1,
                "max": 100,
                "step": 1,
                "val": 10
            },
            {
                "id": "letterSpacing",
                "label": "Letter Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "wordSpacing",
                "label": "Word Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "lineHeight",
                "label": "Line Height",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "fontName",
                "label": "Font Name",
                "type": "select",
                "options": [
                    "hershey_sans"
                ],
                "val": "hershey_sans"
            }
        ]
    },
    "HatchSawtoothPFM": {
        "name": "Hatch Sawtooth",
        "params": [
            {
                "id": "lineSpacing",
                "label": "Linespacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "angle",
                "label": "Angle",
                "type": "range",
                "min": 0.0,
                "max": 225.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "crosshatch",
                "label": "Crosshatch",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "linkEnds",
                "label": "Linkends",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "amplitude",
                "label": "Amplitude",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "minVelocity",
                "label": "Minvelocity",
                "type": "range",
                "min": 0.0,
                "max": 150.0,
                "step": 0.1,
                "val": 30.0
            },
            {
                "id": "maxVelocity",
                "label": "Maxvelocity",
                "type": "range",
                "min": 0.0,
                "max": 450.0,
                "step": 0.1,
                "val": 90.0
            },
            {
                "id": "curveTension",
                "label": "Curvetension",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "SpiralCircularScribblesPFM": {
        "name": "Spiral Circular Scribbles",
        "params": [
            {
                "id": "minRadius",
                "label": "Minradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "maxRadius",
                "label": "Maxradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 8.0
            },
            {
                "id": "angularVelocity",
                "label": "Angularvelocity",
                "type": "range",
                "min": 0.0,
                "max": 150.0,
                "step": 0.1,
                "val": 30.0
            },
            {
                "id": "azimuthAngleMin",
                "label": "Azimuthanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "azimuthAngleMax",
                "label": "Azimuthanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "polarAngleMin",
                "label": "Polaranglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "polarAngleMax",
                "label": "Polaranglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "edgeRetention",
                "label": "Edgeretention",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "edgeThresholdA",
                "label": "Edgethresholda",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "edgeThresholdB",
                "label": "Edgethresholdb",
                "type": "range",
                "min": 0.0,
                "max": 1000.0,
                "step": 0.1,
                "val": 200.0
            },
            {
                "id": "minVelocity",
                "label": "Min Velocity",
                "type": "range",
                "min": 0,
                "max": 50,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxVelocity",
                "label": "Max Velocity",
                "type": "range",
                "min": 0,
                "max": 50,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "targetSpacing",
                "label": "Target Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "mergeTSPPaths",
                "label": "Merge TSP Paths",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "curveTension",
                "label": "Curve Tension",
                "type": "range",
                "min": 0,
                "max": 10,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "StreamlinesFlowFieldPFM": {
        "name": "Streamlines Flow Field",
        "params": [
            {
                "id": "minSpacing",
                "label": "Minspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxSpacing",
                "label": "Maxspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "minLength",
                "label": "Minlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxLength",
                "label": "Maxlength",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "xFrequency",
                "label": "Xfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "yFrequency",
                "label": "Yfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "scaleFrequency",
                "label": "Scalefrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "amplitude",
                "label": "Amplitude",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "ECSDrawingPFM": {
        "name": "E C S Drawing",
        "params": [
            {
                "id": "drawEdges",
                "label": "Drawedges",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "edgeBlur",
                "label": "Edgeblur",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "edgeDetail",
                "label": "Edgedetail",
                "type": "range",
                "min": 0.0,
                "max": 640.0,
                "step": 1,
                "val": 128
            },
            {
                "id": "edgeSimplify",
                "label": "Edgesimplify",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "edgeDistortion",
                "label": "Edgedistortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "drawContours",
                "label": "Drawcontours",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "contourBlur",
                "label": "Contourblur",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "contourDetail",
                "label": "Contourdetail",
                "type": "range",
                "min": 0.0,
                "max": 640.0,
                "step": 1,
                "val": 128
            },
            {
                "id": "contourSimplify",
                "label": "Contoursimplify",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "contourDistortion",
                "label": "Contourdistortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "drawShading",
                "label": "Drawshading",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "shadingAccuracy",
                "label": "Shadingaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDetail",
                "label": "Shadingdetail",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingLength",
                "label": "Shadinglength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            }
        ]
    },
    "PenCalibrationPFM": {
        "name": "Pen Calibration",
        "params": [
            {
                "id": "nibSizeMin",
                "label": "Nibsizemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "nibSizeMax",
                "label": "Nibsizemax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "testCount",
                "label": "Testcount",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 1
            },
            {
                "id": "testSize",
                "label": "Testsize",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 20.0
            },
            {
                "id": "spacingX",
                "label": "Spacingx",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "spacingY",
                "label": "Spacingy",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "rotation",
                "label": "Rotation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineTests",
                "label": "Linetests",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "circleTests",
                "label": "Circletests",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "fontSize",
                "label": "Fontsize",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "MosaicVoronoiPFM": {
        "name": "Mosaic Voronoi",
        "params": [
            {
                "id": "drawOutlines",
                "label": "Drawoutlines",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "tileCount",
                "label": "Tilecount",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 5
            },
            {
                "id": "offsetCells",
                "label": "Offsetcells",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            }
        ]
    },
    "MosaicTriangulationPFM": {
        "name": "Mosaic Triangulation",
        "params": [
            {
                "id": "drawOutlines",
                "label": "Drawoutlines",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "tileCount",
                "label": "Tilecount",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 5
            },
            {
                "id": "offsetCells",
                "label": "Offsetcells",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "triangulateCorners",
                "label": "Triangulatecorners",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "LayersPFM": {
        "name": "Layers",
        "params": []
    },
    "VoronoiShapesPFM": {
        "name": "Voronoi Shapes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "VoronoiTriangulationPFM": {
        "name": "Voronoi Triangulation",
        "params": [
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "triangulateCorners",
                "label": "Triangulatecorners",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "VoronoiStipplingPFM": {
        "name": "Voronoi Stippling",
        "params": [
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "stippleSize",
                "label": "Stipplesize",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            }
        ]
    },
    "VoronoiDiagramPFM": {
        "name": "Voronoi Diagram",
        "params": []
    },
    "SketchSquaresPFM": {
        "name": "Sketch Squares",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            }
        ]
    },
    "SketchWavesPFM": {
        "name": "Sketch Waves",
        "params": []
    },
    "SketchCatmullRomsPFM": {
        "name": "Sketch Catmull Roms",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "curveTension",
                "label": "Curvetension",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "SketchCubicBeziersPFM": {
        "name": "Sketch Cubic Beziers",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "curveTests",
                "label": "Curvetests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 15
            },
            {
                "id": "curveVariation",
                "label": "Curvevariation",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "curveOffsetA",
                "label": "Curveoffseta",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "curveOffsetB",
                "label": "Curveoffsetb",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            }
        ]
    },
    "SketchSobelEdgesPFM": {
        "name": "Sketch Sobel Edges",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "sobelIntensity",
                "label": "Sobelintensity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "sobelAdjust",
                "label": "Sobeladjust",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            }
        ]
    },
    "SketchSweepingCurvesPFM": {
        "name": "Sketch Sweeping Curves",
        "params": [
            {
                "id": "plottingResolution",
                "label": "Plottingresolution",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "randomSeed",
                "label": "Randomseed",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "shouldLiftPen",
                "label": "Shouldliftpen",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "directionality",
                "label": "Directionality",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "clarity",
                "label": "Clarity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "angularity",
                "label": "Angularity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "sobelPower",
                "label": "Sobelpower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "drawingDeltaAngle",
                "label": "Drawingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "lineDensity",
                "label": "Linedensity",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "lineMinLength",
                "label": "Lineminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            },
            {
                "id": "lineMaxLength",
                "label": "Linemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "lineMaxLimit",
                "label": "Linemaxlimit",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": -1
            },
            {
                "id": "angleTests",
                "label": "Angletests",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 12
            },
            {
                "id": "unlimitedTests",
                "label": "Unlimitedtests",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "squiggleMinLength",
                "label": "Squiggleminlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxLength",
                "label": "Squigglemaxlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "squiggleMaxDeviation",
                "label": "Squigglemaxdeviation",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseMin",
                "label": "Erasemin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "eraseMax",
                "label": "Erasemax",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            },
            {
                "id": "eraseRadiusMin",
                "label": "Eraseradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "eraseRadiusMax",
                "label": "Eraseradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "shading",
                "label": "Shading",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "startAngleMin",
                "label": "Startanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngleMax",
                "label": "Startanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "shadingThreshold",
                "label": "Shadingthreshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "shadingDeltaAngle",
                "label": "Shadingdeltaangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "AdaptiveCircularScribblesPFM": {
        "name": "Adaptive Circular Scribbles",
        "params": [
            {
                "id": "minSampleRadius",
                "label": "Minsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "maxSampleRadius",
                "label": "Maxsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "brightness",
                "label": "Brightness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "minRadius",
                "label": "Minradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "maxRadius",
                "label": "Maxradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 8.0
            },
            {
                "id": "minVelocity",
                "label": "Minvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxVelocity",
                "label": "Maxvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "angularVelocity",
                "label": "Angularvelocity",
                "type": "range",
                "min": 0.0,
                "max": 150.0,
                "step": 0.1,
                "val": 30.0
            },
            {
                "id": "azimuthAngleMin",
                "label": "Azimuthanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "azimuthAngleMax",
                "label": "Azimuthanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "polarAngleMin",
                "label": "Polaranglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "polarAngleMax",
                "label": "Polaranglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "edgeRetention",
                "label": "Edgeretention",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "edgeThresholdA",
                "label": "Edgethresholda",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "edgeThresholdB",
                "label": "Edgethresholdb",
                "type": "range",
                "min": 0.0,
                "max": 1000.0,
                "step": 0.1,
                "val": 200.0
            },
            {
                "id": "targetSpacing",
                "label": "Target Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "mergeTSPPaths",
                "label": "Merge TSP Paths",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "curveTension",
                "label": "Curve Tension",
                "type": "range",
                "min": 0,
                "max": 10,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "AdaptiveStipplingPFM": {
        "name": "Adaptive Stippling",
        "params": [
            {
                "id": "minSampleRadius",
                "label": "Minsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "maxSampleRadius",
                "label": "Maxsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "brightness",
                "label": "Brightness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "stippleSize",
                "label": "Stipplesize",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            }
        ]
    },
    "AdaptiveTreePFM": {
        "name": "Adaptive Tree",
        "params": [
            {
                "id": "minSampleRadius",
                "label": "Minsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "maxSampleRadius",
                "label": "Maxsampleradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "brightness",
                "label": "Brightness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "createCurves",
                "label": "Createcurves",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "AdaptiveLettersPFM": {
        "name": "Adaptive Letters",
        "params": [
            {
                "id": "word",
                "label": "Word",
                "type": "select",
                "options": [
                    "dbv3"
                ],
                "val": "dbv3"
            },
            {
                "id": "fontSize",
                "label": "Font Size",
                "type": "range",
                "min": 1,
                "max": 100,
                "step": 1,
                "val": 10
            },
            {
                "id": "letterSpacing",
                "label": "Letter Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "wordSpacing",
                "label": "Word Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "lineHeight",
                "label": "Line Height",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "fontName",
                "label": "Font Name",
                "type": "select",
                "options": [
                    "hershey_sans"
                ],
                "val": "hershey_sans"
            }
        ]
    },
    "LBGCircularScribblesPFM": {
        "name": "L B G Circular Scribbles",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRadius",
                "label": "Minradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "maxRadius",
                "label": "Maxradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 8.0
            },
            {
                "id": "minVelocity",
                "label": "Minvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxVelocity",
                "label": "Maxvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "angularVelocity",
                "label": "Angularvelocity",
                "type": "range",
                "min": 0.0,
                "max": 150.0,
                "step": 0.1,
                "val": 30.0
            },
            {
                "id": "azimuthAngleMin",
                "label": "Azimuthanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "azimuthAngleMax",
                "label": "Azimuthanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "polarAngleMin",
                "label": "Polaranglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "polarAngleMax",
                "label": "Polaranglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "edgeRetention",
                "label": "Edgeretention",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "edgeThresholdA",
                "label": "Edgethresholda",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "edgeThresholdB",
                "label": "Edgethresholdb",
                "type": "range",
                "min": 0.0,
                "max": 1000.0,
                "step": 0.1,
                "val": 200.0
            },
            {
                "id": "targetSpacing",
                "label": "Target Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "mergeTSPPaths",
                "label": "Merge TSP Paths",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "curveTension",
                "label": "Curve Tension",
                "type": "range",
                "min": 0,
                "max": 10,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "LBGTriangulationPFM": {
        "name": "L B G Triangulation",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "triangulateCorners",
                "label": "Triangulatecorners",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "LBGDashesPFM": {
        "name": "L B G Dashes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "LBGDiagramPFM": {
        "name": "L B G Diagram",
        "params": []
    },
    "LBGTSPPFM": {
        "name": "L B G T S P",
        "params": [
            {
                "id": "stippleRadiusMin",
                "label": "Stippleradiusmin",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "stippleRadiusMax",
                "label": "Stippleradiusmax",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "density",
                "label": "Density",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "maxIterations",
                "label": "Maxiterations",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            },
            {
                "id": "cacheResult",
                "label": "Cacheresult",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "mergeTSPPaths",
                "label": "Mergetsppaths",
                "type": "checkbox",
                "val": true
            }
        ]
    },
    "GridDashesPFM": {
        "name": "Grid Dashes",
        "params": [
            {
                "id": "shapeType",
                "label": "Shape Type",
                "type": "select",
                "options": [
                    "Circle",
                    "Square",
                    "Star",
                    "Triangle",
                    "Random"
                ],
                "val": "Circle"
            },
            {
                "id": "alignRotation",
                "label": "Align Rotation",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRotation",
                "label": "Min Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxRotation",
                "label": "Max Rotation",
                "type": "range",
                "min": 0,
                "max": 360,
                "step": 1,
                "val": 360
            },
            {
                "id": "fillSize",
                "label": "Fill Size",
                "type": "range",
                "min": 0.1,
                "max": 100,
                "step": 0.1,
                "val": 4.0
            }
        ]
    },
    "HatchCircularScribblesPFM": {
        "name": "Hatch Circular Scribbles",
        "params": [
            {
                "id": "lineSpacing",
                "label": "Linespacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "angle",
                "label": "Angle",
                "type": "range",
                "min": 0.0,
                "max": 225.0,
                "step": 0.1,
                "val": 45.0
            },
            {
                "id": "crosshatch",
                "label": "Crosshatch",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "linkEnds",
                "label": "Linkends",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "minRadius",
                "label": "Minradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "maxRadius",
                "label": "Maxradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 8.0
            },
            {
                "id": "minVelocity",
                "label": "Minvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxVelocity",
                "label": "Maxvelocity",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "angularVelocity",
                "label": "Angularvelocity",
                "type": "range",
                "min": 0.0,
                "max": 150.0,
                "step": 0.1,
                "val": 30.0
            },
            {
                "id": "azimuthAngleMin",
                "label": "Azimuthanglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "azimuthAngleMax",
                "label": "Azimuthanglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "polarAngleMin",
                "label": "Polaranglemin",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": -180.0
            },
            {
                "id": "polarAngleMax",
                "label": "Polaranglemax",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 180.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.5
            },
            {
                "id": "edgeRetention",
                "label": "Edgeretention",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "edgeThresholdA",
                "label": "Edgethresholda",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "edgeThresholdB",
                "label": "Edgethresholdb",
                "type": "range",
                "min": 0.0,
                "max": 1000.0,
                "step": 0.1,
                "val": 200.0
            },
            {
                "id": "targetSpacing",
                "label": "Target Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "mergeTSPPaths",
                "label": "Merge TSP Paths",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "curveTension",
                "label": "Curve Tension",
                "type": "range",
                "min": 0,
                "max": 10,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "SpiralSawtoothPFM": {
        "name": "Spiral Sawtooth",
        "params": []
    },
    "StreamlinesEdgeFieldPFM": {
        "name": "Streamlines Edge Field",
        "params": [
            {
                "id": "minSpacing",
                "label": "Minspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxSpacing",
                "label": "Maxspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "minLength",
                "label": "Minlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxLength",
                "label": "Maxlength",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "xFrequency",
                "label": "Xfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "yFrequency",
                "label": "Yfrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "scaleFrequency",
                "label": "Scalefrequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "amplitude",
                "label": "Amplitude",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "edgePower",
                "label": "Edgepower",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "etfIterations",
                "label": "Etfiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "etfRadius",
                "label": "Etfradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "postBlurIterations",
                "label": "Postbluriterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 5
            },
            {
                "id": "postBlurRadius",
                "label": "Postblurradius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            }
        ]
    },
    "StreamlinesSuperformulaPFM": {
        "name": "Streamlines Superformula",
        "params": [
            {
                "id": "minSpacing",
                "label": "Minspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxSpacing",
                "label": "Maxspacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "minLength",
                "label": "Minlength",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "maxLength",
                "label": "Maxlength",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 0.1,
                "val": 100.0
            },
            {
                "id": "tone",
                "label": "Tone",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "distortion",
                "label": "Distortion",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "startAngle",
                "label": "Startangle",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "centreX",
                "label": "Centrex",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "centreY",
                "label": "Centrey",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "xScale",
                "label": "Xscale",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "yScale",
                "label": "Yscale",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "frequency",
                "label": "Frequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "curvature",
                "label": "Curvature",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "sineFactor",
                "label": "Sinefactor",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "cosFactor",
                "label": "Cosfactor",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "SVGConverterPFM": {
        "name": "S V G Converter",
        "params": [
            {
                "id": "shapeClipping",
                "label": "Shapeclipping",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "deriveDrawingSet",
                "label": "Derivedrawingset",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "shapeFilling",
                "label": "Shapefilling",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "spacing",
                "label": "Spacing",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 3.0
            },
            {
                "id": "minRotation",
                "label": "Minrotation",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "maxRotation",
                "label": "Maxrotation",
                "type": "range",
                "min": 0.0,
                "max": 360.0,
                "step": 0.1,
                "val": 360.0
            },
            {
                "id": "linkEnds",
                "label": "Linkends",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "crosshatch",
                "label": "Crosshatch",
                "type": "checkbox",
                "val": false
            }
        ]
    },
    "VoronoiLettersPFM": {
        "name": "Voronoi Letters",
        "params": [
            {
                "id": "word",
                "label": "Word",
                "type": "select",
                "options": [
                    "dbv3"
                ],
                "val": "dbv3"
            },
            {
                "id": "fontSize",
                "label": "Font Size",
                "type": "range",
                "min": 1,
                "max": 100,
                "step": 1,
                "val": 10
            },
            {
                "id": "letterSpacing",
                "label": "Letter Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "wordSpacing",
                "label": "Word Spacing",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "lineHeight",
                "label": "Line Height",
                "type": "range",
                "min": 0,
                "max": 100,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "fontName",
                "label": "Font Name",
                "type": "select",
                "options": [
                    "hershey_sans"
                ],
                "val": "hershey_sans"
            }
        ]
    },
    "MosaicRectanglesPFM": {
        "name": "Mosaic Rectangles",
        "params": [
            {
                "id": "drawOutlines",
                "label": "Drawoutlines",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "squareTiles",
                "label": "Squaretiles",
                "type": "checkbox",
                "val": false
            },
            {
                "id": "columns",
                "label": "Columns",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 8
            },
            {
                "id": "rows",
                "label": "Rows",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 8
            },
            {
                "id": "columnPaddingPercent",
                "label": "Columnpaddingpercent",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            },
            {
                "id": "rowPaddingPercent",
                "label": "Rowpaddingpercent",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            }
        ]
    },
    "MosaicSegmentsPFM": {
        "name": "Mosaic Segments",
        "params": [
            {
                "id": "drawOutlines",
                "label": "Drawoutlines",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "pointDensity",
                "label": "Pointdensity",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 0.1,
                "val": 500.0
            },
            {
                "id": "pointLimit",
                "label": "Pointlimit",
                "type": "range",
                "min": 0.0,
                "max": 500000.0,
                "step": 1,
                "val": 100000
            },
            {
                "id": "luminancePower",
                "label": "Luminancepower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "densityPower",
                "label": "Densitypower",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "voronoiIterations",
                "label": "Voronoiiterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "voronoiAccuracy",
                "label": "Voronoiaccuracy",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            },
            {
                "id": "ignoreWhite",
                "label": "Ignorewhite",
                "type": "checkbox",
                "val": true
            },
            {
                "id": "tileCount",
                "label": "Tilecount",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 5
            },
            {
                "id": "offsetCells",
                "label": "Offsetcells",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.0
            },
            {
                "id": "segments",
                "label": "Segments",
                "type": "range",
                "min": 0.0,
                "max": 2500.0,
                "step": 1,
                "val": 500
            },
            {
                "id": "iterations",
                "label": "Iterations",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "compactness",
                "label": "Compactness",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 20.0
            }
        ]
    },
    "MosaicCustomPFM": {
        "name": "Mosaic Custom",
        "params": []
    }
};

const FILTER_SCHEMA = {
    "Dirty Border": {
        "name": "Dirty Border",
        "params": [
            {
                "id": "width",
                "label": "Width",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 15
            }
        ]
    },
    "Custom Overlay": {
        "name": "Custom Overlay",
        "params": [
            {
                "id": "opacity",
                "label": "Opacity",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "Box Blur": {
        "name": "Box Blur",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 3
            }
        ]
    },
    "Emboss Edges": {
        "name": "Emboss Edges",
        "params": []
    },
    "Gaussian Blur": {
        "name": "Gaussian Blur",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 3
            }
        ]
    },
    "Glow": {
        "name": "Glow",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 7
            },
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "High Pass": {
        "name": "High Pass",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 3
            }
        ]
    },
    "Lens Blur": {
        "name": "Lens Blur",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 4
            }
        ]
    },
    "Maximum": {
        "name": "Maximum",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 1
            }
        ]
    },
    "Median": {
        "name": "Median",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 2
            }
        ]
    },
    "Minimum": {
        "name": "Minimum",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 1
            }
        ]
    },
    "Motion Blur Fast": {
        "name": "Motion Blur Fast",
        "params": [
            {
                "id": "length",
                "label": "Length",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 15
            }
        ]
    },
    "Motion Blur Slow": {
        "name": "Motion Blur Slow",
        "params": [
            {
                "id": "length",
                "label": "Length",
                "type": "range",
                "min": 0.0,
                "max": 125.0,
                "step": 1,
                "val": 25
            }
        ]
    },
    "Sharpen": {
        "name": "Sharpen",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.5
            }
        ]
    },
    "Simple Blur": {
        "name": "Simple Blur",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 1
            }
        ]
    },
    "Smart Blur": {
        "name": "Smart Blur",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 4
            },
            {
                "id": "sigma",
                "label": "Sigma",
                "type": "range",
                "min": 0.0,
                "max": 375.0,
                "step": 0.1,
                "val": 75.0
            }
        ]
    },
    "Unsharp Mask": {
        "name": "Unsharp Mask",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 2.0
            },
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.5
            }
        ]
    },
    "Adjust HSB": {
        "name": "Adjust HSB",
        "params": [
            {
                "id": "hue",
                "label": "Hue",
                "type": "range",
                "min": -1.0,
                "max": 1.0,
                "step": 0.01,
                "val": 0.0
            },
            {
                "id": "sat",
                "label": "Sat",
                "type": "range",
                "min": -1.0,
                "max": 1.0,
                "step": 0.01,
                "val": 0.0
            },
            {
                "id": "bright",
                "label": "Bright",
                "type": "range",
                "min": -1.0,
                "max": 1.0,
                "step": 0.01,
                "val": 0.0
            }
        ]
    },
    "Adjust RGB": {
        "name": "Adjust RGB",
        "params": [
            {
                "id": "r",
                "label": "R",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "g",
                "label": "G",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            },
            {
                "id": "b",
                "label": "B",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            }
        ]
    },
    "Contrast": {
        "name": "Contrast",
        "params": [
            {
                "id": "contrast",
                "label": "Contrast",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.2
            }
        ]
    },
    "Exposure": {
        "name": "Exposure",
        "params": [
            {
                "id": "exposure",
                "label": "Exposure",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.0
            },
            {
                "id": "offset",
                "label": "Offset",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 20.0
            }
        ]
    },
    "Gain": {
        "name": "Gain",
        "params": [
            {
                "id": "gain",
                "label": "Gain",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 1.1
            },
            {
                "id": "bias",
                "label": "Bias",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            }
        ]
    },
    "Gamma": {
        "name": "Gamma",
        "params": [
            {
                "id": "gamma",
                "label": "Gamma",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.2
            }
        ]
    },
    "Gray Out": {
        "name": "Gray Out",
        "params": []
    },
    "Invert": {
        "name": "Invert",
        "params": []
    },
    "Levels": {
        "name": "Levels",
        "params": [
            {
                "id": "minLevel",
                "label": "Minlevel",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 0
            },
            {
                "id": "maxLevel",
                "label": "Maxlevel",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            }
        ]
    },
    "Mix Channels": {
        "name": "Mix Channels",
        "params": []
    },
    "Posterize": {
        "name": "Posterize",
        "params": [
            {
                "id": "levels",
                "label": "Levels",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 8
            }
        ]
    },
    "Quantize": {
        "name": "Quantize",
        "params": [
            {
                "id": "numColors",
                "label": "Numcolors",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 4
            }
        ]
    },
    "Rescale": {
        "name": "Rescale",
        "params": [
            {
                "id": "scale",
                "label": "Scale",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "Solarize": {
        "name": "Solarize",
        "params": [
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 635.0,
                "step": 1,
                "val": 127
            }
        ]
    },
    "Transparency": {
        "name": "Transparency",
        "params": [
            {
                "id": "opacity",
                "label": "Opacity",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.8
            }
        ]
    },
    "Diffuse": {
        "name": "Diffuse",
        "params": [
            {
                "id": "scale",
                "label": "Scale",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 20.0
            }
        ]
    },
    "Displace": {
        "name": "Displace",
        "params": [
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 1.0
            }
        ]
    },
    "Kaleidoscope": {
        "name": "Kaleidoscope",
        "params": [
            {
                "id": "sides",
                "label": "Sides",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 4
            }
        ]
    },
    "Marble": {
        "name": "Marble",
        "params": [
            {
                "id": "turbulence",
                "label": "Turbulence",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 0.1,
                "val": 50.0
            }
        ]
    },
    "Ripple": {
        "name": "Ripple",
        "params": [
            {
                "id": "frequency",
                "label": "Frequency",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            }
        ]
    },
    "Shear": {
        "name": "Shear",
        "params": [
            {
                "id": "shearX",
                "label": "Shearx",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.2
            },
            {
                "id": "shearY",
                "label": "Sheary",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.2
            }
        ]
    },
    "Swim": {
        "name": "Swim",
        "params": [
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 7.0
            }
        ]
    },
    "Detect Edges": {
        "name": "Detect Edges",
        "params": [
            {
                "id": "threshold1",
                "label": "Threshold1",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 1,
                "val": 100
            },
            {
                "id": "threshold2",
                "label": "Threshold2",
                "type": "range",
                "min": 0.0,
                "max": 1000.0,
                "step": 1,
                "val": 200
            }
        ]
    },
    "Laplace": {
        "name": "Laplace",
        "params": []
    },
    "Chrome": {
        "name": "Chrome",
        "params": [
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "Feedback": {
        "name": "Feedback",
        "params": [
            {
                "id": "zoom",
                "label": "Zoom",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 0.9
            }
        ]
    },
    "Glint": {
        "name": "Glint",
        "params": [
            {
                "id": "length",
                "label": "Length",
                "type": "range",
                "min": 0.0,
                "max": 105.0,
                "step": 1,
                "val": 21
            }
        ]
    },
    "Mirror": {
        "name": "Mirror",
        "params": []
    },
    "Chroma Key": {
        "name": "Chroma Key",
        "params": [
            {
                "id": "tolerance",
                "label": "Tolerance",
                "type": "range",
                "min": 0.0,
                "max": 500.0,
                "step": 1,
                "val": 100
            }
        ]
    },
    "Color Halftone": {
        "name": "Color Halftone",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            }
        ]
    },
    "Crystallize": {
        "name": "Crystallize",
        "params": [
            {
                "id": "size",
                "label": "Size",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 10.0
            }
        ]
    },
    "Pointillize": {
        "name": "Pointillize",
        "params": [
            {
                "id": "size",
                "label": "Size",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 0.1,
                "val": 5.0
            }
        ]
    },
    "Scratches": {
        "name": "Scratches",
        "params": [
            {
                "id": "count",
                "label": "Count",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 1
            }
        ]
    },
    "Noise": {
        "name": "Noise",
        "params": [
            {
                "id": "amount",
                "label": "Amount",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 10.0
            }
        ]
    },
    "Contours": {
        "name": "Contours",
        "params": [
            {
                "id": "levels",
                "label": "Levels",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            }
        ]
    },
    "Dissolve": {
        "name": "Dissolve",
        "params": [
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 1200.0,
                "step": 1,
                "val": 240
            }
        ]
    },
    "Drop Shadow": {
        "name": "Drop Shadow",
        "params": [
            {
                "id": "offsetX",
                "label": "Offsetx",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "offsetY",
                "label": "Offsety",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            },
            {
                "id": "opacity",
                "label": "Opacity",
                "type": "range",
                "min": 0.0,
                "max": 10.0,
                "step": 0.1,
                "val": 0.5
            }
        ]
    },
    "Emboss": {
        "name": "Emboss",
        "params": []
    },
    "Flare": {
        "name": "Flare",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 250.0,
                "step": 1,
                "val": 50
            }
        ]
    },
    "Oil": {
        "name": "Oil",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 10
            }
        ]
    },
    "Rays": {
        "name": "Rays",
        "params": [
            {
                "id": "length",
                "label": "Length",
                "type": "range",
                "min": 0.0,
                "max": 155.0,
                "step": 1,
                "val": 31
            }
        ]
    },
    "Shape Burst": {
        "name": "Shape Burst",
        "params": []
    },
    "Sparkle": {
        "name": "Sparkle",
        "params": [
            {
                "id": "radius",
                "label": "Radius",
                "type": "range",
                "min": 0.0,
                "max": 100.0,
                "step": 1,
                "val": 5
            }
        ]
    },
    "Stamp": {
        "name": "Stamp",
        "params": [
            {
                "id": "threshold",
                "label": "Threshold",
                "type": "range",
                "min": 0.0,
                "max": 635.0,
                "step": 1,
                "val": 127
            }
        ]
    },
    "Threshold": {
        "name": "Threshold",
        "params": [
            {
                "id": "lowerThreshold",
                "label": "Lowerthreshold",
                "type": "range",
                "min": 0.0,
                "max": 635.0,
                "step": 1,
                "val": 127
            },
            {
                "id": "upperThreshold",
                "label": "Upperthreshold",
                "type": "range",
                "min": 0.0,
                "max": 1275.0,
                "step": 1,
                "val": 255
            }
        ]
    }
};
