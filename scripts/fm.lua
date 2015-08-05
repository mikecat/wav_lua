-- パラメータ : 周波数[Hz](省略時1000) beta(省略時周波数/10)

function process(data, sample_rate, parameters)
	freq = tonumber(parameters[1] or 1000)
	beta = tonumber(parameters[2] or (freq / 10))
	result = {}
	for c = 1, #data do
		result[c] = {}
		sekibun = 0
		for i = 1, #data[c] do
			sekibun = sekibun + data[c][i] / sample_rate
			result[c][i] = math.cos((i - 1) * math.pi * 2 / (sample_rate / freq) + beta * sekibun)
		end
	end
	return result
end
