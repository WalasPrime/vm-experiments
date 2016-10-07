vars = Variables(None, ARGUMENTS)
vars.Add('DEBUG', 'Debug mode (1-enabled)', 0)
env = Environment(variables = vars, CPPDEFINES={'DEBUG' : '${DEBUG}'})

env.Program('bin/c_machine'+('_debug' if env['DEBUG'] == '1' else ''), Glob('src/*.cpp'), CPPATH='./src')