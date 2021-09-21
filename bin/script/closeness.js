function draw_centrality_pack_layout(parent, data) {
    var format = d3.format(",d"),
        color = d3.scaleOrdinal(d3.schemeCategory20);
        
    var svg = parent.append('svg')
        .attr("id", "Betweenness")
        .attr("width", 800)
        .attr("height", 800);
        
    var diameter = +svg.attr("width");
    var g = svg.append("g");

    var pack = d3.pack()
        .size([diameter, diameter])
        .padding(1.5);

    var root = d3.hierarchy({children: data})
        .sum(d => d.value);

    var node = g.selectAll(".node")
        .data(pack(root).descendants())
        .enter()
        .append("g")
        .attr("class", d => d.children ? "node" : "leaf node")
        .attr("transform", d => "translate(" + d.x + "," + d.y + ")");

    node.append("title")
        .text(d => d.data.name + " Value: " + format(d.value));

    node.append("circle")
        .attr("r", d => d.children ? 0 : d.r)
        .style("fill", d => color(d.data.type));

    node.filter(d => !d.children)
        .append("text")
        .attr("dy", "0.3em")
        .text(d => d.r >= 10 ? d.data.name : "");
}


draw_centrality_pack_layout(d3.select("#closeness"), closeness);