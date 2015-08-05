-- パラメータ : 周波数[Hz](省略時1000)

function process(data, sample_rate, parameters)
	freq = tonumber(parameters[1] or 1000)
	interval = math.ceil(sample_rate / freq)
	result = {}
	for c = 1, #data do
		result[c] = {}
		for i = 1, #data[c], interval do
			sum = 0
			count = 0
			for j = i, i + interval - 1 do
				if j <= #data[c] then
					sum = sum + data[c][j] + 1
					count = count + 1
				end
			end
			value = (sum / count) / 2
			for j = 0, interval do
				if i + j <= #data[c] then
					if j / interval <= value then
						result[c][i + j] = 1
					else
						result[c][i + j] = 0
					end
				end
			end
		end
	end
	return result
end
