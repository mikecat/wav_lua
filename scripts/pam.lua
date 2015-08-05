-- パラメータ : 周波数[Hz](省略時1000) デューティー比(省略時0.5)

function process(data, sample_rate, parameters)
	freq = tonumber(parameters[1] or 1000)
	ratio = tonumber(parameters[2] or 0.5)
	result = {}
	for c = 1, #data do
		result[c] = {}
		for i = 1, #data[c] do
			ip, fp = math.modf((i - 1) / (sample_rate / freq))
			if fp >= ratio then
				result[c][i] = 0
			else
				result[c][i] = data[c][i]
			end
		end
	end
	return result
end
