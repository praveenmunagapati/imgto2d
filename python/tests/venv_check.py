import sys
print('python:', sys.executable)

for mod in ('numpy','serial','vpype'):
    try:
        m = __import__(mod)
        ver = getattr(m, '__version__', 'ok')
        print(mod, 'OK', ver)
    except Exception as e:
        print(mod, 'ERR', type(e).__name__, str(e))
