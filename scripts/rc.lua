-- パラメータ : R[Ω](省略時1) C[F](省略時1)

function process(data, sample_rate, parameters)
	r = tonumber(parameters[1] or 1)
	c = tonumber(parameters[2] or 1)
	result = {}
	for ch = 1, #data do
		result[ch] = {}
		denki = 0
		for i = 1, #data[ch] do
			c_denatu = denki / c
			denryu = (data[ch][i] - c_denatu) / r
			denki = denki + denryu / sample_rate
			result[ch][i] = denki / c
		end
	end
	return result
end
