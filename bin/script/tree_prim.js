var width=1500;
var height=600;
var up_type = [ " ","游戏","动画","舞蹈","音乐","鬼畜","影视","数码","生活","国创","时尚","科技","娱乐","纪录片","广告","番剧","电影","电视剧" ];
var svg=d3.select("body")
.append("svg")
.attr("width",width)
.attr("height",height);

var force=d3.layout.force()
.nodes(nodes)
.links(tree_edges)
.size([width,height])
.linkDistance(50)
.charge([-100]);

force.start();
var svg_edges=svg.selectAll("line")
.data(tree_edges)
.enter()
.append("line")
.style("stroke","#ccc")
.style("stroke-width",1);

var color=d3.scale.category20();

var svg_nodes=svg.selectAll("circle")
.data(nodes)
.enter()
.append("circle")
.attr("r",5)
.style("fill",function(d){
    return color(d.type);
})
.call(force.drag);

var set_Events=svg_nodes
.on("mouseenter",function(d){
    d3.select(this)
    .transition()
    .attr("r",15);
    d3.select("p")
    .html("Up主名称:<a target=\"_blank\" href=\"https://space.bilibili.com/"+d.uid.toString()+"/\">"+d.name+"</a>,分区:"+up_type[d.type]);
})
.on("mouseleave",function(){
    d3.select(this)
    .transition()
    .attr("r",5);
})

force.on("tick",function(){
    svg_edges.attr("x1",function(d){ return d.source.x;})
    .attr("y1",function(d){ return d.source.y;})
    .attr("x2",function(d){ return d.target.x;})
    .attr("y2",function(d){ return d.target.y;});

    svg_nodes.attr("cx",function(d){ return d.x;})
    .attr("cy",function(d){ return d.y;});

  

})