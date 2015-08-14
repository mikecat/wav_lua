-- パラメータ: ファイル名 長さ[秒](省略時1) サンプリングレート(省略時44100)

function process(data, sample_rate, parameters)
	length = tonumber(parameters[2] or 1)
	rate = tonumber(parameters[3] or 44100)
	filedata = {}
	width = 0
	for v in io.lines(parameters[1]) do
		table.insert(filedata, tonumber(v))
		width = math.max(width, math.abs(tonumber(v)))
	end
	res = {}
	for i = 1, length * rate do
		res[i] = filedata[i % #filedata + 1] / width
	end
	return {res}, rate
end
