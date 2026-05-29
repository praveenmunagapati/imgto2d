document.addEventListener('DOMContentLoaded', () => {
    const uploadInput = document.getElementById('image-upload');
    const fileName = document.getElementById('file-name');
    const generateBtn = document.getElementById('generate-btn');
    const previewImage = document.getElementById('preview-image');
    const placeholder = document.getElementById('placeholder');
    const loader = document.getElementById('loader');

    let currentImagePath = '';

    // ==========================================
    // INITIALIZE ALGORITHMS
    // ==========================================
    const algoSelect = document.getElementById('algorithm-select');
    const algoControls = document.getElementById('dynamic-controls');

    const sortedKeys = Object.keys(PFM_SCHEMA).sort((a, b) => {
        return PFM_SCHEMA[a].name.localeCompare(PFM_SCHEMA[b].name);
    });

    sortedKeys.forEach(key => {
        const opt = document.createElement('option');
        opt.value = key;
        opt.textContent = PFM_SCHEMA[key].name;
        algoSelect.appendChild(opt);
    });

    if (sortedKeys.length > 0) {
        algoSelect.value = sortedKeys[0];
    }

    function renderAlgoControls(algoKey) {
        algoControls.innerHTML = '';
        if (!PFM_SCHEMA[algoKey]) return; // Failsafe
        const schema = PFM_SCHEMA[algoKey].params;
        schema.forEach(p => {
            const group = document.createElement('div');
            group.className = 'control-group';
            
            if (p.type === 'checkbox') {
                group.classList.add('checkbox-group');
                group.innerHTML = `
                    <label style="color: var(--text-main); font-weight: normal; text-transform: none;">
                        <input type="checkbox" id="param-${p.id}" ${p.val ? 'checked' : ''}>
                        ${p.label}
                    </label>
                `;
            } else if (p.type === 'text') {
                group.innerHTML = `
                    <label>${p.label}</label>
                    <input type="text" id="param-${p.id}" value="${p.val}" style="width: 100%; background: rgba(0,0,0,0.3); border: 1px solid var(--panel-border); color: white; padding: 8px; border-radius: 4px;">
                `;
            } else {
                group.innerHTML = `
                    <label>${p.label}: <span id="val-${p.id}">${p.val}</span></label>
                    <input type="range" id="param-${p.id}" min="${p.min}" max="${p.max}" step="${p.step}" value="${p.val}">
                `;
                const input = group.querySelector('input');
                const span = group.querySelector('span');
                input.addEventListener('input', (e) => {
                    span.textContent = e.target.value;
                });
            }
            algoControls.appendChild(group);
        });
    }

    algoSelect.addEventListener('change', (e) => {
        renderAlgoControls(e.target.value);
    });
    
    renderAlgoControls(algoSelect.value);

    // ==========================================
    // INITIALIZE FILTERS
    // ==========================================
    const filterSelect = document.getElementById('filter-select');
    const addFilterBtn = document.getElementById('add-filter-btn');
    const activeFiltersList = document.getElementById('active-filters-list');
    let activeFilters = [];

    if (typeof FILTER_SCHEMA !== 'undefined') {
        filterSelect.innerHTML = '';
        Object.keys(FILTER_SCHEMA).forEach(filterName => {
            const opt = document.createElement('option');
            opt.value = filterName;
            opt.textContent = FILTER_SCHEMA[filterName].name;
            filterSelect.appendChild(opt);
        });
    }

    addFilterBtn.addEventListener('click', () => {
        const filterName = filterSelect.value;
        const schema = FILTER_SCHEMA[filterName];
        if (!schema) return;

        const filterObj = {
            name: filterName,
            params: {}
        };

        const item = document.createElement('div');
        item.style.background = '#222';
        item.style.padding = '10px';
        item.style.marginBottom = '10px';
        item.style.borderRadius = '5px';
        item.style.position = 'relative';

        const header = document.createElement('h4');
        header.textContent = schema.name;
        header.style.margin = '0 0 10px 0';
        item.appendChild(header);

        const removeBtn = document.createElement('button');
        removeBtn.textContent = '×';
        removeBtn.style.position = 'absolute';
        removeBtn.style.top = '10px';
        removeBtn.style.right = '10px';
        removeBtn.style.background = 'transparent';
        removeBtn.style.border = 'none';
        removeBtn.style.color = '#ff4444';
        removeBtn.style.cursor = 'pointer';
        removeBtn.style.fontSize = '18px';
        
        removeBtn.addEventListener('click', () => {
            item.remove();
            activeFilters = activeFilters.filter(f => f !== filterObj);
        });
        item.appendChild(removeBtn);

        schema.params.forEach(param => {
            filterObj.params[param.id] = param.val;
            const row = document.createElement('div');
            row.style.display = 'flex';
            row.style.justifyContent = 'space-between';
            row.style.alignItems = 'center';
            row.style.marginBottom = '5px';

            const label = document.createElement('label');
            label.textContent = param.label;
            label.style.fontSize = '12px';

            const input = document.createElement('input');
            input.type = param.type;
            if (param.type === 'range' || param.type === 'number') {
                input.min = param.min;
                input.max = param.max;
                input.step = param.step;
                input.value = param.val;
                input.style.width = '60%';
                
                const valDisplay = document.createElement('span');
                valDisplay.textContent = param.val;
                valDisplay.style.fontSize = '12px';
                valDisplay.style.marginLeft = '5px';
                valDisplay.style.width = '30px';

                input.addEventListener('input', (e) => {
                    const v = parseFloat(e.target.value);
                    valDisplay.textContent = v;
                    filterObj.params[param.id] = v;
                });
                
                row.appendChild(label);
                const inputWrap = document.createElement('div');
                inputWrap.style.display = 'flex';
                inputWrap.style.alignItems = 'center';
                inputWrap.appendChild(input);
                inputWrap.appendChild(valDisplay);
                row.appendChild(inputWrap);
            } else if (param.type === 'checkbox') {
                input.checked = param.val;
                input.addEventListener('change', (e) => {
                    filterObj.params[param.id] = e.target.checked;
                });
                row.appendChild(label);
                row.appendChild(input);
            }
            item.appendChild(row);
        });

        activeFiltersList.appendChild(item);
        activeFilters.push(filterObj);
    });

    // ==========================================
    // TABS & PENS
    // ==========================================
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
            e.target.classList.add('active');
            document.getElementById(e.target.dataset.tab).classList.add('active');
        });
    });

    const penList = document.getElementById('pen-list');
    const addPenBtn = document.getElementById('add-pen-btn');
    
    function addPen(color = "#000000", weight = 1.0) {
        const row = document.createElement('div');
        row.className = 'pen-row';
        row.innerHTML = `
            <input type="color" value="${color}">
            <input type="number" step="0.1" value="${weight}" title="Weight">
            <button class="remove-pen">&times;</button>
        `;
        row.querySelector('.remove-pen').addEventListener('click', () => row.remove());
        penList.appendChild(row);
    }
    
    addPen(); // Default pen
    addPenBtn.addEventListener('click', () => addPen());

    // ==========================================
    // UPLOAD & GENERATE
    // ==========================================
    uploadInput.addEventListener('change', async (e) => {
        const file = e.target.files[0];
        if (!file) return;

        fileName.textContent = "Uploading...";
        generateBtn.disabled = true;

        try {
            const response = await fetch('/api/upload', {
                method: 'POST',
                body: file
            });

            if (!response.ok) throw new Error("Upload failed");

            const data = await response.json();
            currentImagePath = data.path;
            
            const objectUrl = URL.createObjectURL(file);
            previewImage.src = objectUrl;
            previewImage.style.display = 'block';
            placeholder.style.display = 'none';

            fileName.textContent = file.name;
            generateBtn.disabled = false;
        } catch (error) {
            fileName.textContent = "Upload failed";
            console.error(error);
        }
    });

    generateBtn.addEventListener('click', async () => {
        if (!currentImagePath) return;

        // 1. Gather Pens
        const pens = [];
        penList.querySelectorAll('.pen-row').forEach(row => {
            pens.push({
                "enabled": true,
                "color": row.querySelector('input[type="color"]').value,
                "weight": parseFloat(row.querySelector('input[type="number"]').value)
            });
        });

        // 2. Gather Dynamic Algo Settings
        const pfmSettings = {};
        const algoKey = algoSelect.value;
        const algoName = PFM_SCHEMA[algoKey] ? PFM_SCHEMA[algoKey].name : "Unknown";
        
        if (PFM_SCHEMA[algoKey]) {
            PFM_SCHEMA[algoKey].params.forEach(p => {
                const el = document.getElementById(`param-${p.id}`);
                if (!el) return;
                if (p.type === 'checkbox') {
                    pfmSettings[p.id] = el.checked;
                } else if (p.type === 'text') {
                    pfmSettings[p.id] = el.value;
                } else {
                    pfmSettings[p.id] = parseFloat(el.value);
                }
            });
        }

        // 3. Gather Filters
        const filter_chain = activeFilters.map(f => {
            const payload = {
                type: f.name,
                enabled: true
            };
            Object.assign(payload, f.params);
            return payload;
        });

        // 4. Build Config
        const config = {
            "preset_sub_type": algoName,
            "preset_name": "Default",
            "image_path": currentImagePath,
            "filter_chain": filter_chain,
            "drawing_area": {
                "use_original_sizing": false,
                "width_mm": parseFloat(document.getElementById('setup-width').value) || 210.0,
                "height_mm": parseFloat(document.getElementById('setup-height').value) || 297.0,
                "input_units": "mm",
                "scaling_mode": document.getElementById('setup-scaling').value,
                "background_colour": document.getElementById('setup-bg').value
            },
            "pen_settings": {
                "color_separation": document.getElementById('pens-separation').value,
                "distribution_type": document.getElementById('pens-distribution').value,
                "pens": pens.length > 0 ? pens : [{"enabled": true, "color": "#000000", "weight": 1.0}]
            },
            "optimization_settings": {
                "enable_simplifying": document.getElementById('opt-simplify').checked,
                "enable_merging": document.getElementById('opt-merge').checked,
                "enable_sorting": document.getElementById('opt-sort').checked
            },
            "gcode_settings": {
                "offset_x": parseFloat(document.getElementById('export-offsetx').value) || 0.0,
                "offset_y": parseFloat(document.getElementById('export-offsety').value) || 0.0,
                "curve_flatness": parseFloat(document.getElementById('export-flatness').value) || 0.1
            },
            "pfm_name": algoKey,
            "pfm_settings": pfmSettings
        };

        generateBtn.disabled = true;
        previewImage.style.opacity = '0.3';
        loader.style.display = 'flex';
        
        const progressContainer = document.getElementById('progressContainer');
        const progressBar = document.getElementById('progressBar');
        const progressText = document.getElementById('progressText');
        progressContainer.style.display = 'block';
        progressText.style.display = 'block';
        progressBar.style.width = '0%';
        progressText.textContent = 'Starting...';

        let pollInterval = setInterval(async () => {
            try {
                const res = await fetch('/api/status');
                if (res.ok) {
                    const status = await res.json();
                    progressBar.style.width = status.progress + '%';
                    progressText.textContent = status.message || 'Processing...';
                }
            } catch (e) {
                // Ignore
            }
        }, 300);

        try {
            const response = await fetch('/api/generate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            });

            if (!response.ok) {
                const errText = await response.text();
                throw new Error(errText);
            }

            const blob = await response.blob();
            const url = URL.createObjectURL(blob);
            previewImage.src = url;
            
        } catch (error) {
            alert('Generation failed: ' + error.message);
            console.error(error);
        } finally {
            clearInterval(pollInterval);
            loader.style.display = 'none';
            progressContainer.style.display = 'none';
            progressText.style.display = 'none';
            previewImage.style.opacity = '1';
            generateBtn.disabled = false;
        }
    });
});
