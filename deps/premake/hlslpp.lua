hlslpp = {
	source = path.join(dependencies.basePath, "hlslpp"),
}

function hlslpp.import()
	dependson "hlslpp"
	links { "hlslpp" }
	hlslpp.includes()
end

function hlslpp.includes()
	includedirs {
		path.join(hlslpp.source, "include"),
	}
end

function hlslpp.project()
	project "hlslpp"
		language "C"

		hlslpp.includes()
		files
		{
			path.join(hlslpp.source, "include/***")
		}

		warnings "Off"
		kind "None"
end

table.insert(dependencies, hlslpp)
