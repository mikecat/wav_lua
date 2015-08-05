-- パラメータ : 周波数[Hz](省略時1000) 変調指数(省略時1)

function process(data, sample_rate, parameters)
	freq = tonumber(parameters[1] or 1000)
	m = tonumber(parameters[2] or 1)
	result = {}
	for c = 1, #data do
		result[c] = {}
		for i = 1, #data[c] do
			result[c][i] = (1 + m * data[c][i]) * math.sin((i - 1) * math.pi * 2 / (sample_rate / freq)) / (1 + m)
		end
	end
	return result
end
